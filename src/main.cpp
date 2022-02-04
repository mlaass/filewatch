#include <efsw/FileSystem.hpp>
#include <efsw/System.hpp>
#include <efsw/efsw.hpp>
#include <iostream>

#include <iostream>
#include <signal.h>

bool STOP = false;

void sigend(int signal) {
  std::cout << std::endl << "Bye bye" << std::endl;
  STOP = true;
}

// Inherits from the abstract listener class, and implements the the file action
// handler
class UpdateListener : public efsw::FileWatchListener {
public:
  void handleFileAction(efsw::WatchID watchid, const std::string &dir,
                        const std::string &filename, efsw::Action action,
                        std::string oldFilename) override {
    switch (action) {
    case efsw::Actions::Add:
      std::cout << "DIR (" << dir << ") FILE (" << filename
                << ") has event Added" << std::endl;
      break;
    case efsw::Actions::Delete:
      std::cout << "DIR (" << dir << ") FILE (" << filename
                << ") has event Delete" << std::endl;
      break;
    case efsw::Actions::Modified:
      std::cout << "DIR (" << dir << ") FILE (" << filename
                << ") has event Modified" << std::endl;
      break;
    case efsw::Actions::Moved:
      std::cout << "DIR (" << dir << ") FILE (" << filename
                << ") has event Moved from (" << oldFilename << ")"
                << std::endl;
      break;
    default:
      std::cout << "Should never happen!" << std::endl;
    }
  }
};

int main(int argc, char **argv) {

  signal(SIGABRT, sigend);
  signal(SIGINT, sigend);
  signal(SIGTERM, sigend);

  std::cout << "Press ^C to exit demo" << std::endl;

  // Create the file system watcher instance
  // efsw::FileWatcher allow a first boolean parameter that indicates if it
  // should start with the generic file watcher instead of the platform specific
  // backend

  efsw::FileWatcher fileWatcher(false);
  fileWatcher.followSymlinks(false);
  fileWatcher.allowOutOfScopeLinks(false);

  // Create the instance of your efsw::FileWatcherListener implementation
  UpdateListener *listener = new UpdateListener();

  // Add a folder to watch, and get the efsw::WatchID
  // It will watch the /tmp folder recursively ( the third parameter indicates
  // that is recursive ) Reporting the files and directories changes to the
  // instance of the listener
  efsw::WatchID watchID = fileWatcher.addWatch(".", listener, true);

  // Start watching asynchronously the directories
  fileWatcher.watch();

  while (!STOP) {
    efsw::System::sleep(100);
  }
  return 0;
}