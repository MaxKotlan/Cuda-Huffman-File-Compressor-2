#ifndef STARTUP_H
#define STARTUP_H

#include <string>

class Startup
{
public:
	static void Init(int, char**);
	static Startup& Instance();
	static void Destroy();

	int MaximumRam() { return _maximumRam; };

private:
	static Startup* MyInstance(Startup* start);

	Startup(int, char**) : _maximumRam(0) {};
	~Startup(){ }

	void ParseCommandLine(int, char**);
	unsigned int _maximumRam;
};

inline void Startup::ParseCommandLine(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--maxram" && i+1<argc) { _maximumRam = stoi(std::string(argv[i + 1])); }

	}
}

inline void Startup::Init(int argc, char** argv)
{
	Startup* ptr = new Startup(argc, argv);
	ptr->ParseCommandLine(argc, argv);
	MyInstance(ptr);
}

inline Startup& Startup::Instance()
{
	return *MyInstance(NULL);
}

inline Startup* Startup::MyInstance(Startup* ptr)
{
	static Startup* myInstance = NULL;
	if (ptr)
		myInstance = ptr;
	return myInstance;
}

inline void Startup::Destroy()
{
	Startup* pLog = MyInstance(NULL);
	if (pLog)
		delete pLog;
}


#endif