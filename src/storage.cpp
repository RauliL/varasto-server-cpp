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
#include "./storage.hpp"
#include "./utils.hpp"

namespace varasto
{
  Storage::get_all_entries_type
  Storage::GetAllEntries(const key_type& ns) const
  {
    const auto keys = GetAllKeys(ns);

    if (keys)
    {
      std::vector<mapped_type> entries;

      for (const auto& key : keys.value())
      {
        const auto value = Get(ns, key);

        if (value)
        {
          if (*value)
          {
            entries.push_back(std::make_pair(key, *(*value)));
          }
        } else {
          return get_all_entries_type::error(value.error());
        }
      }

      return get_all_entries_type::ok(entries);
    }

    return get_all_entries_type::error(keys.error());
  }

  Storage::update_result_type
  Storage::Update(
    const key_type& ns,
    const key_type& key,
    const value_type& value
  )
  {
    const auto old_value_result = Get(ns, key);

    if (old_value_result)
    {
      auto old_value = old_value_result.value();

      if (old_value)
      {
        const auto new_value = utils::patch(*old_value, value);
        const auto set_result = Set(ns, key, new_value);

        if (set_result)
        {
          return update_result_type::ok(*old_value);
        }

        return update_result_type::error(set_result.error());
      }

      return update_result_type::ok(std::nullopt);
    }

    return update_result_type::error(old_value_result.error());
  }
}
