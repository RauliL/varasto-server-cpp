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
#include <fstream>

#include <peelo/json/formatter.hpp>
#include <peelo/json/parser.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "./filesystem-storage.hpp"
#include "./slug.hpp"

namespace varasto
{
  using peelo::json::format;
  using peelo::json::parse_object;
  using peelo::unicode::encoding::utf8::decode;

  FilesystemStorage::FilesystemStorage(const path_type& root)
    : m_root(root) {}

  Storage::get_result_type
  FilesystemStorage::Get(
    const key_type& ns,
    const key_type& key
  ) const
  {
    const auto entry_and_path_result = GetEntryAndPath(ns, key);

    if (entry_and_path_result)
    {
      return get_result_type::ok(entry_and_path_result->second);
    }

    return get_result_type::error(entry_and_path_result.error());
  }

  Storage::get_all_keys_type
  FilesystemStorage::GetAllKeys(
    const key_type& ns
  ) const
  {
    const auto ns_path_result = GetNamespacePath(ns);

    if (ns_path_result)
    {
      const auto& ns_path = ns_path_result.value();
      std::vector<key_type> keys;

      if (std::filesystem::is_directory(ns_path))
      {
        for (const auto& entry : std::filesystem::directory_iterator(ns_path))
        {
          if (std::filesystem::is_regular_file(entry))
          {
            keys.push_back(entry.path().filename().string());
          }
        }
      }

      return get_all_keys_type::ok(keys);
    }

    return get_all_keys_type::error(ns_path_result.error());
  }

  Storage::set_result_type
  FilesystemStorage::Set(
    const key_type& ns,
    const key_type& key,
    const value_type& value
  )
  {
    const auto path_result = GetEntryPath(ns, key);

    if (path_result)
    {
      const auto ns_path = path_result->parent_path();

      if (!std::filesystem::is_directory(ns_path))
      {
        if (!std::filesystem::create_directories(ns_path))
        {
          return set_result_type::error(
            "Failed to create namespace directory."
          );
        }
      }

      {
        std::ofstream file(path_result.value());

        if (!file.good())
        {
          return set_result_type::error("Failed to open file.");
        }
        file << format(value);
        file.close();
      }

      return set_result_type::ok(true);
    }

    return set_result_type::error(path_result.error());
  }

  Storage::delete_result_type
  FilesystemStorage::Delete(
    const key_type& ns,
    const key_type& key
  )
  {
    const auto entry_and_path_result = GetEntryAndPath(ns, key);

    if (entry_and_path_result)
    {
      const auto& entry_and_path = entry_and_path_result.value();
      const auto& path = entry_and_path.first;

      if (std::filesystem::remove(path))
      {
        const auto parent = path.parent_path();

        // Remove parent directory if it's empty.
        if (
          std::filesystem::is_directory(parent) &&
          std::filesystem::is_empty(parent)
        )
        {
          std::filesystem::remove(parent);
        }

        return delete_result_type::ok(entry_and_path.second);
      }

      return delete_result_type::ok(std::nullopt);
    }

    return delete_result_type::error(entry_and_path_result.error());
  }

  Storage::delete_namespace_result_type
  FilesystemStorage::DeleteNamespace(const key_type& ns)
  {
    const auto path_result = GetNamespacePath(ns);

    if (path_result && std::filesystem::is_directory(*path_result))
    {
      const auto list_result = GetAllEntries(ns);

      if (list_result)
      {
        std::filesystem::remove_all(*path_result);

        return delete_namespace_result_type::ok(*list_result);
      }

      return delete_namespace_result_type::error(list_result.error());
    }

    return delete_namespace_result_type::ok(std::nullopt);
  }

  FilesystemStorage::get_path_result_type
  FilesystemStorage::GetNamespacePath(const key_type& ns) const
  {
    if (!is_valid_slug(ns))
    {
      return get_path_result_type::error("Invalid namespace: " + ns);
    }

    return get_path_result_type::ok(m_root / ns);
  }

  FilesystemStorage::get_path_result_type
  FilesystemStorage::GetEntryPath(
    const key_type& ns,
    const key_type& key
  ) const
  {
    if (!is_valid_slug(ns))
    {
      return get_path_result_type::error("Invalid namespace: " + ns);
    }
    else if (!is_valid_slug(key))
    {
      return get_path_result_type::error("Invalid key: " + ns);
    }

    return get_path_result_type::ok(m_root / ns / key);
  }

  FilesystemStorage::get_entry_and_path_result_type
  FilesystemStorage::GetEntryAndPath(
    const key_type& ns,
    const key_type& key
  ) const
  {
    const auto path_result = GetEntryPath(ns, key);

    if (path_result)
    {
      const auto& path = path_result.value();

      if (std::filesystem::is_regular_file(path))
      {
        std::ifstream file(path);

        if (!file.good())
        {
          return get_entry_and_path_result_type::error("Failed to open file.");
        }

        const auto buffer = std::string(
          std::istreambuf_iterator<char>(file),
          std::istreambuf_iterator<char>()
        );

        file.close();

        const auto result = parse_object(decode(buffer));

        if (result)
        {
          return get_entry_and_path_result_type::ok(
            std::make_pair(path, result.value())
          );
        }

        return get_entry_and_path_result_type::error(result.error().what());
      }

      return get_entry_and_path_result_type::ok(
        std::make_pair(path, std::nullopt)
      );
    }

    return get_entry_and_path_result_type::error(path_result.error());
  }
}
