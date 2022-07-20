#pragma once
#include <vector>
#include <string>
#include <memory>
#include <fmt/core.h>
#include <fmt/color.h>
#ifdef _UNICODE
  #undef _UNICODE
  #include <getopt.h>
  #define _UNICODE
#endif
#include <stdbool.h>
#include <console/utf8.hpp>

namespace console
{
  // base class for attribute
  struct BaseAttr
  {
    // constructor/destructor
    BaseAttr(const std::string& _cmd_small,
             const std::string& _cmd_full,
             const std::string& _desc,
             bool _mandatory=false) : 
      cmd_small(_cmd_small),
      cmd_full(_cmd_full),
      desc(_desc),
      mandatory(_mandatory) {}
    virtual ~BaseAttr() {};

    // check if this attribute has an argument
    bool has_arg() const
    {
      return (cmd_small.find(":") != -1);
    }

    // generic attributes parameters
    std::string cmd_small;
    std::string cmd_full;
    std::string desc;
    bool mandatory;
  };

  // specific attribute class
  template<typename T>
  struct Attr final : public BaseAttr
  {
    Attr(const std::string& _cmd_small,
         const std::string& _cmd_full,
         const std::string& _desc,
         T& _arg,
         bool _mandatory = false) :
      BaseAttr(_cmd_small, _cmd_full, _desc, _mandatory),
      arg(_arg) {}

    // direct access to the original argument
    T& arg;
  };

  // command-line parser
  class parser final
  {
  public:
    parser(const std::string& program_name,
           const std::string& program_version) :
      m_name(program_name),
      m_version(program_version),
      m_args()
    {
      // add default program options
      m_args.push_back(std::make_unique<BaseAttr>("h", "help", "display the program help"));
      m_args.push_back(std::make_unique<BaseAttr>("v", "version", "display the program version"));
    }
    
    // register command with argument
    template<typename T>
    void add(const std::string& cmd_small,
             const std::string& cmd_full,
             const std::string& desc, 
             T& arg, 
             bool mandatory = false)
    {
      m_args.push_back(std::make_unique<Attr<T>>(cmd_small, cmd_full, desc, arg, mandatory));
    }

    // display program usage
    void print_usage() const
    {
      // lambda function to display a colored tag
      auto add_tag = [](const fmt::v8::color color, const std::string& text) {
        return fmt::format(fmt::fg(color) | fmt::emphasis::bold, "[{}]", text);
      };

      // display program command
      fmt::print("{} {}\n",
        fmt::format(fmt::emphasis::bold, "usage: {}.exe", m_name),
        add_tag(fmt::color::green, "options"));

      // calc max size of cmd_full and desc - for alignment
      std::size_t max_size_cmd_full = 0;
      std::size_t max_size_desc = 0;
      for (const auto& arg : m_args)
      {
        if (arg->cmd_full.size() > max_size_cmd_full)
          max_size_cmd_full = arg->cmd_full.size();
        if (arg->desc.size() > max_size_desc)
          max_size_desc = arg->desc.size();
      }

      // display specific options
      std::string format;
      format = "  -{}, ";
      format += " --{:<" + std::to_string(max_size_cmd_full + 4) + "} ";
      format += "{:<" + std::to_string(max_size_desc + 2) + "} ";
      format += "{}\n";
      for (const auto& arg : m_args)
      {
        fmt::print(format,
                   arg->cmd_small[0],
                   arg->cmd_full,
                   arg->desc,
                   arg->mandatory ? add_tag(fmt::color::orange, "mandatory") : "");
      }
    }

    // parse command-line arguments using get-opt
    bool parse(int argc, char** argv)
    {
      // construct get-opt long arguments definition
      std::vector<struct option> long_opt;
      for (const auto& arg : m_args)
      {
        long_opt.push_back({ arg->cmd_full.c_str(),
                             arg->has_arg(),
                             nullptr,
                             arg->cmd_small[0] });
      }
      long_opt.push_back({ 0, 0 ,0 ,0 });

      // construct get-opt short arguments definition
      std::string short_opt;
      for (const auto& arg : m_args)
        short_opt += arg->cmd_small;

      // parse arguments
      try
      {
        int opt;
        while ((opt = getopt_long(argc, argv, short_opt.c_str(), long_opt.data(), nullptr)) != -1)
        {
          // handle generic options
          switch (opt)
          {
          case 'h':
            print_usage();
            exit(0);
            break;
          case 'v':
            fmt::print("{}.exe (version: v{})\n", m_name, m_version);
            exit(0);
            break;
          case ':':
            throw std::runtime_error("option needs a value");
            break;
          default:
            break;
          }

          // handle specific options
          bool found = false;
          for (auto& arg : m_args)
          {
            if (arg->cmd_small[0] == opt)
            {
              if (arg->has_arg())
              {
                auto attr = dynamic_cast<Attr<std::string>*>(arg.get());
                if (!attr)
                  throw std::runtime_error("invalid format for argument: --" + arg->cmd_full);
                attr->arg = utf8::to_utf8(optarg);
              }
              else
              {
                auto attr = dynamic_cast<Attr<bool>*>(arg.get());
                if (!attr)
                  throw std::runtime_error("invalid format for argument: --" + arg->cmd_full);
                attr->arg = true;
              }
              found = true;
              break;
            }
          }
          if (!found)
            throw std::runtime_error("unknown option");
        }
      }
      catch (const std::exception& ex)
      {
        fmt::print("{} {}\n", 
          fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "),
          ex.what());
        return false;
      }
      return true;
    }

  private:
    std::string m_name;
    std::string m_version;
    std::vector<std::unique_ptr<BaseAttr>> m_args;
  };
}
