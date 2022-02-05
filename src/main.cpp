#include <efsw/FileSystem.hpp>
#include <efsw/System.hpp>
#include <efsw/efsw.hpp>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <signal.h>
#include <sstream>

#include "httplib.hpp"
#include "json.hpp"
#include "quick_arg_parser.hpp"

namespace fs = std::filesystem;

fs::path get_diffpath(fs::path basepath, fs::path newpath) {
  fs::path diffpath;

  fs::path tmppath = newpath;
  while (!fs::equivalent(tmppath, basepath)) {
    diffpath = tmppath.stem() / diffpath;
    tmppath = tmppath.parent_path();
  }
  return diffpath;
}

bool STOP = false;

void sigend(int signal) {
  std::cout << std::endl << "Bye bye" << std::endl;
  STOP = true;
}

// Inherits from the abstract listener class, and implements the the file action
// handler
class UpdateListener : public efsw::FileWatchListener {
  std::map<std::string, size_t> hashes;

public:
  std::function<void(std::string, std::string)> callback;
  fs::path parent;
  bool verbose;

  std::string getRelative(std::string dir, std::string filename) {
    fs::path diffpath = get_diffpath(parent, dir);
    diffpath /= filename;
    return diffpath.string();
  }

  void update(std::string dir, std::string filename) {
    fs::path fullpath = dir;
    fullpath /= filename;
    std::ifstream t(fullpath.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string content = buffer.str();

    // ignore empty because of inotify
    if (content.size() == 0)
      return;

    auto relative_fn = getRelative(dir, filename);

    auto hash = std::hash<std::string>{}(content);
    if (hashes.count(filename) != 0) {
      if (hashes[relative_fn] != hash) {
        hashes[relative_fn] = hash;
        callback(relative_fn, content);
      }
    } else {
      hashes[relative_fn] = hash;
      callback(relative_fn, content);
    }
  }

  void handleFileAction(efsw::WatchID watchid, const std::string &dir,
                        const std::string &filename, efsw::Action action,
                        std::string oldFilename) override {
    switch (action) {
    case efsw::Actions::Add:
      if (verbose)
        std::cout << "DIR (" << dir << ") FILE (" << filename
                  << ") has event Added" << std::endl;
      update(dir, filename);
      break;
    case efsw::Actions::Delete:
      if (verbose)
        std::cout << "DIR (" << dir << ") FILE (" << filename
                  << ") has event Delete" << std::endl;
      break;
    case efsw::Actions::Modified:
      if (verbose)
        std::cout << "DIR (" << dir << ") FILE (" << filename
                  << ") has event Modified" << std::endl;
      update(dir, filename);
      break;
    case efsw::Actions::Moved:
      if (verbose)
        std::cout << "DIR (" << dir << ") FILE (" << filename
                  << ") has event Moved from (" << oldFilename << ")"
                  << std::endl;
      if (hashes.count(getRelative(dir, oldFilename)) != 0) {
        hashes[getRelative(dir, filename)] =
            hashes[getRelative(dir, oldFilename)];
      }
      update(dir, filename);
      break;
    default:
      if (verbose)
        std::cout << "Should never happen!" << std::endl;
    }
  }
};

struct Args : MainArguments<Args> {
  std::string folder = argument(0) = ".";
  bool verbose = option("verbose", 'v', "Verbose output");
  bool recursive = option("recursive", 'r', "Watch files recursively") = false;
  int interval = option("interval", 'i', "Update interval in milliseconds") =
      1000;
  std::string host = option("host", 'h', "Host URL including port") =
      "localhost:8080";
  std::string post_url =
      option("post", 'p', "Host URL to post updated files to") = "/post";
  std::string ping_url =
      option("ping", 'g', "Host URL to ping every intervall") = "";
};

int main(int argc, char **argv) {

  Args args{{argc, argv}};
  if (args.verbose) {
    std::cout << "Folder: " << args.folder << std::endl;
    std::cout << "Recursive: " << args.recursive << std::endl;
    std::cout << "Host: " << args.host << std::endl;
    std::cout << "Post URL: " << args.post_url << std::endl;
    std::cout << "Ping URL: " << args.ping_url << std::endl;
  }

  std::string post_url = args.host + args.post_url;
  std::string ping_url = args.host + args.ping_url;

  signal(SIGABRT, sigend);
  signal(SIGINT, sigend);
  signal(SIGTERM, sigend);

  std::cout << "Press ^C to exit" << std::endl;

  efsw::FileWatcher fileWatcher(false);
  fileWatcher.followSymlinks(false);
  fileWatcher.allowOutOfScopeLinks(false);

  UpdateListener listener;
  listener.verbose = args.verbose;

  httplib::Client cli(args.host);

  listener.callback = [&](std::string fn, std::string content) {
    std::cout << "Update File: \'" << fn << "\'" << std::endl;

    nlohmann::json body = {{"relative_path", fn}, {"contents", content}};
    cli.Post(args.post_url.c_str(), {}, body.dump(0), "application/json");
  };

  listener.parent = fs::absolute(args.folder);
  std::cout << "Folder: " << listener.parent.string() << std::endl;

  efsw::WatchID watchID =
      fileWatcher.addWatch(listener.parent.string(), &listener, args.recursive);

  // Start watching asynchronously the directories
  fileWatcher.watch();

  while (!STOP) {
    if (args.ping_url.size() > 0)
      cli.Post(args.ping_url.c_str());
    efsw::System::sleep(args.interval);
  }
  return 0;
}