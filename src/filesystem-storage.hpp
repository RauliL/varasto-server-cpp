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
#pragma once

#include <filesystem>

#include "./storage.hpp"

namespace varasto
{
  class FilesystemStorage : public Storage
  {
  public:
    using path_type = std::filesystem::path;

    using get_path_result_type = peelo::result<
      path_type,
      std::string
    >;
    using get_entry_and_path_result_type = peelo::result<
      std::pair<
        path_type,
        std::optional<value_type>
      >,
      std::string
    >;

    FilesystemStorage(const path_type& root);

    FilesystemStorage(const FilesystemStorage&) = default;
    FilesystemStorage(FilesystemStorage&&) = default;
    FilesystemStorage& operator=(const FilesystemStorage&) = default;
    FilesystemStorage& operator=(FilesystemStorage&&) = default;

    get_result_type Get(
      const key_type& ns,
      const key_type& key
    ) const;

    get_all_keys_type GetAllKeys(
      const key_type& ns
    ) const;

    set_result_type Set(
      const key_type& ns,
      const key_type& key,
      const value_type& value
    );

    delete_result_type Delete(
      const key_type& ns,
      const key_type& key
    );

    delete_result_type DeleteNamespace(
      const key_type& ns
    );

  private:
    get_path_result_type GetNamespacePath(
      const key_type& ns
    ) const;

    get_path_result_type GetEntryPath(
      const key_type& ns,
      const key_type& key
    ) const;

    get_entry_and_path_result_type GetEntryAndPath(
      const key_type& ns,
      const key_type& key
    ) const;

  private:
    path_type m_root;
  };
}
