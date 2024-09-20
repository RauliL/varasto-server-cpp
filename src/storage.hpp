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

#include <optional>

#include <peelo/json/value.hpp>
#include <peelo/result.hpp>

namespace varasto
{
  class Storage
  {
  public:
    using key_type = std::string;
    using value_type = peelo::json::object::ptr;
    using mapped_type = std::pair<key_type, value_type>;

    using get_result_type = peelo::result<
      std::optional<value_type>,
      std::string
    >;
    using get_all_keys_type = peelo::result<
      std::vector<key_type>,
      std::string
    >;
    using get_all_entries_type = peelo::result<
      std::vector<mapped_type>,
      std::string
    >;
    using set_result_type = peelo::result<
      bool,
      std::string
    >;
    using update_result_type = peelo::result<
      std::optional<value_type>,
      std::string
    >;
    using delete_result_type = peelo::result<
      std::optional<value_type>,
      std::string
    >;

    virtual get_result_type Get(
      const key_type& ns,
      const key_type& key
    ) const = 0;

    virtual get_all_keys_type GetAllKeys(
      const key_type& ns
    ) const = 0;

    get_all_entries_type GetAllEntries(
      const key_type& ns
    ) const;

    virtual set_result_type Set(
      const key_type& ns,
      const key_type& key,
      const value_type& value
    ) = 0;

    update_result_type Update(
      const key_type& ns,
      const key_type& key,
      const value_type& value
    );

    virtual delete_result_type Delete(
      const key_type& ns,
      const key_type& key
    ) = 0;

    virtual delete_result_type DeleteNamespace(
      const key_type& ns
    ) = 0;
  };
}
