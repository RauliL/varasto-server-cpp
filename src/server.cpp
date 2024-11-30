/*
 * Copyright (c) 2024, Rauli Laine
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <httplib.h>
#include <peelo/json/formatter.hpp>
#include <peelo/json/parser.hpp>
#include <peelo/unicode/encoding/utf8.hpp>
#include <uuid.h>

#include "./filesystem-storage.hpp"
#include "./server.hpp"

namespace varasto
{
  using httplib::Request;
  using httplib::Response;
  using httplib::Server;
  using peelo::json::format;
  using peelo::json::object;
  using peelo::json::parse_object;
  using peelo::json::string;
  using peelo::unicode::encoding::utf8::decode;

  static const char* content_type = "application/json; charset=utf-8";

  static std::string
  generate_uuid()
  {
    std::random_device device;
    std::array<int, std::mt19937::state_size> seed_data;
    std::generate(
      std::begin(seed_data),
      std::end(seed_data),
      std::ref(device)
    );
    std::seed_seq sequence(std::begin(seed_data), std::end(seed_data));
    std::mt19937 generator(sequence);
    uuids::uuid_random_generator gen { generator };

    return uuids::to_string(gen());
  }

  static void
  send_error_message(Response& res, const std::string& message, int status)
  {
    object::container_type properties;

    properties[U"error"] = string::make(decode(message));
    res.status = status;
    res.set_content(format(object::make(properties)), content_type);
  }

  static std::optional<Storage::value_type>
  parse_object(const Request& req, Response& res)
  {
    const auto result = parse_object(decode(req.body));

    if (result)
    {
      return *result;
    }

    send_error_message(res, "Value is not an object.", 400);

    return std::nullopt;
  }

  static void
  handle_entry_list(
    const Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto result = storage.GetAllEntries(ns);

    if (result)
    {
      object::container_type properties;

      for (const auto& entry : result.value())
      {
        properties[decode(entry.first)] = entry.second;
      }
      res.set_content(
        format(object::make(properties)),
        content_type
      );
    } else {
      send_error_message(res, result.error(), 500);
    }
  }

  static void
  handle_entry_get(
    const Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto& key = req.path_params.at("key");
    const auto result = storage.Get(ns, key);

    if (result)
    {
      const auto& value = result.value();

      if (value)
      {
        res.set_content(format(*value), content_type);
      } else {
        send_error_message(res, "Entry does not exist.", 404);
      }
    } else {
      send_error_message(res, result.error(), 500);
    }
  }

  static void
  do_set(
    Storage& storage,
    Response& res,
    const Storage::key_type& ns,
    const Storage::key_type& key,
    const Storage::value_type& value
  )
  {
    const auto result = storage.Set(ns, key, value);

    if (result)
    {
      res.status = 201;
      res.set_content(format(value), content_type);
    } else {
      send_error_message(res, result.error(), 500);
    }
  }

  static void
  handle_entry_set(
    Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto& key = req.path_params.at("key");
    Storage::value_type value;

    if (const auto value = parse_object(req, res))
    {
      do_set(storage, res, ns, key, *value);
    }
  }

  static void
  handle_entry_insert(
    Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");

    if (const auto value = parse_object(req, res))
    {
      const auto key = generate_uuid();
      const auto result = storage.Set(ns, key, *value);

      if (result)
      {
        object::container_type properties;

        properties[U"key"] = string::make(decode(key));
        res.status = 201;
        res.set_content(
          format(object::make(properties)),
          content_type
        );
      } else {
        send_error_message(res, result.error(), 500);
      }
    }
  }

  static void
  handle_entry_update(
    Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto& key = req.path_params.at("key");

    if (const auto value = parse_object(req, res))
    {
      const auto result = storage.Update(ns, key, *value);

      if (result)
      {
        const auto new_value = result.value();

        if (new_value)
        {
          res.status = 201;
          res.set_content(format(*new_value), content_type);
        } else {
          send_error_message(res, "Entry does not exist.", 404);
        }
      } else {
        send_error_message(res, result.error(), 500);
      }
    }
  }

  static void
  handle_namespace_delete(
    Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto result = storage.DeleteNamespace(ns);

    if (result)
    {
      if (const auto entries = *result)
      {
        object::container_type properties;

        for (const auto& entry : *entries)
        {
          properties[decode(entry.first)] = entry.second;
        }
        res.status = 201;
        res.set_content(format(object::make(properties)), content_type);
      } else {
        send_error_message(res, "Namespace does not exist.", 404);
      }
    } else {
      send_error_message(res, result.error(), 500);
    }
  }

  static void
  handle_entry_delete(
    Storage& storage,
    const Request& req,
    Response& res
  )
  {
    const auto& ns = req.path_params.at("namespace");
    const auto& key = req.path_params.at("key");
    const auto result = storage.Delete(ns, key);

    if (result)
    {
      const auto& value = result.value();

      if (value)
      {
        res.status = 201;
        res.set_content(format(*value), content_type);
      } else {
        send_error_message(res, "Entry does not exist.", 404);
      }
    } else {
      send_error_message(res, result.error(), 500);
    }
  }

  void
  run_server(const ServerOptions& options)
  {
    FilesystemStorage storage(options.root);
    Server server;

    if (!std::filesystem::is_directory(options.root))
    {
      std::cerr << "Root directory "
                << options.root
                << " does not exist."
                << std::endl;
      std::exit(EXIT_FAILURE);
    }

    server.Get(
      "/",
      [](const Request& req, Response& res)
      {
        res.set_content("{}", content_type);
      }
    );
    server.Get(
      "/:namespace",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_list(storage, req, res);
      }
    );
    server.Post(
      "/:namespace",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_insert(storage, req, res);
      }
    );
    server.Get(
      "/:namespace/:key",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_get(storage, req, res);
      }
    );
    server.Post(
      "/:namespace/:key",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_set(storage, req, res);
      }
    );
    server.Patch(
      "/:namespace/:key",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_update(storage, req, res);
      }
    );
    server.Delete(
      "/:namespace",
      [&storage](const Request& req, Response& res)
      {
        handle_namespace_delete(storage, req, res);
      }
    );
    server.Delete(
      "/:namespace/:key",
      [&storage](const Request& req, Response& res)
      {
        handle_entry_delete(storage, req, res);
      }
    );

    std::cout << "Listening on http://"
              << options.hostname
              << ":"
              << options.port
              << std::endl;

    if (!server.listen(options.hostname, options.port))
    {
      std::cerr << "Failed to listen on "
                << options.hostname
                << ":"
                << options.port
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }
}
