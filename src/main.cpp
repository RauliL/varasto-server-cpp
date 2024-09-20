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
#include <cstring>
#include <iostream>

#include "./server.hpp"

using varasto::ServerOptions;

static void
display_usage(std::ostream& output, const char* executable)
{
  output << std::endl
         << "Usage: "
         << executable
         << " [switches] [root-directory]"
         << std::endl
         << "   -h             Hostname to listen to. (Default: localhost)"
         << std::endl
         << "   -p             Port to listen to. (Default: 8080)"
         << std::endl
         << "   --version      Print the version."
         << std::endl
         << "   --help         Display this message."
         << std::endl
         << std::endl;
}

static void
parse_args(int argc, char** argv, ServerOptions& options)
{
  int offset = 1;

  options.hostname = "localhost";
  options.port = 8080;
  options.root = std::filesystem::current_path() / "data";

  while (offset < argc)
  {
    auto arg = argv[offset++];

    if (!*arg)
    {
      continue;
    }
    else if (*arg != '-')
    {
      options.root = arg;
      break;
    }
    else if (!arg[1])
    {
      break;
    }
    else if (arg[1] == '-')
    {
      if (!std::strcmp(arg, "--help"))
      {
        display_usage(std::cout, argv[0]);
        std::exit(EXIT_SUCCESS);
      }
      else if (!std::strcmp(arg, "--version"))
      {
        std::cout << "Varasto server 0.0.1" << std::endl;
        std::exit(EXIT_SUCCESS);
      } else {
        std::cerr << "Unrecognized switch: " << arg << std::endl;
        display_usage(std::cerr, argv[0]);
        std::exit(EXIT_FAILURE);
      }
    }
    for (int i = 1; arg[i]; ++i)
    {
      switch (arg[i])
      {
        case 'h':
          if (offset < argc)
          {
            options.hostname = argv[offset++];
          } else {
            std::cerr << "Argument expected for the -h option." << std::endl;
            display_usage(std::cerr, argv[0]);
            std::exit(EXIT_FAILURE);
          }
          break;

        case 'p':
          if (offset < argc)
          {
            try
            {
              options.port = std::stoi(argv[offset++]);
            }
            catch (const std::exception& e)
            {
              std::cerr << "Invalid argument for the -p option." << std::endl;
              std::exit(EXIT_FAILURE);
            }
          } else {
            std::cerr << "Argument expected for the -p option." << std::endl;
            display_usage(std::cerr, argv[0]);
            std::exit(EXIT_FAILURE);
          }
          break;

        default:
          std::cerr << "Unrecognized switch: " << arg[i] << std::endl;
          display_usage(std::cerr, argv[0]);
          std::exit(EXIT_FAILURE);
      }
    }
  }

  if (offset < argc)
  {
    std::cerr << "Too many arguments given." << std::endl;
    display_usage(std::cerr, argv[0]);
    std::exit(EXIT_FAILURE);
  }
}

int
main(int argc, char** argv)
{
  ServerOptions options;

  parse_args(argc, argv, options);
  varasto::run_server(options);

  return 0;
}
