#ifndef STARTUP_H
#define STARTUP_H

#include <string>

typedef enum {
	Compressing,
	Decompressing
} FileOperation;

class Startup
{
public:
	static void Init(int, char**);
	static Startup& Instance();
	static void Destroy();

	void Help();
	int FileBufferSize()          { return _fileBufferSize; };
	FileOperation fileOperation() { return _operation; };
	std::string fileName()        { return _fileName; };
	bool fileBufferDisabled()     { return _disableFileBuffer; };
	unsigned int fileBufferSize() { return _fileBufferSize; };
	bool UseCuda()                { return _cuda; };

private:
	static Startup* MyInstance(Startup* start);

	Startup(int, char**) : 
		_fileBufferSize(1024), 
		_disableFileBuffer(false),
		_cuda(false)
	{};
	~Startup(){ }

	void ParseCommandLine(int, char**);
	FileOperation _operation;
	std::string   _fileName;
	std::string   _executableName;
		     bool _cuda;
	         bool _disableFileBuffer;
	 unsigned int _fileBufferSize;
};

inline void Startup::ParseCommandLine(int argc, char** argv) {
	_executableName = std::string(argv[0]);
	if (argc < 3) Help();
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--compress" && i+1<argc) { 
			_operation = Compressing;
			_fileName = std::string(argv[i + 1]);
		}
		if (std::string(argv[i]) == "--decompress" && i+1<argc) { 
			_operation = Decompressing;
			_fileName = std::string(argv[i + 1]);
		}
		if (std::string(argv[i]) == "--cuda")           			 { _cuda = true; }
		if (std::string(argv[i]) == "--disablefilebuffer")           { _disableFileBuffer = true; }
		if (std::string(argv[i]) == "--filebuffersize" & i+1 < argc) { _fileBufferSize    = stoi(std::string(argv[i+1])); }
	}
}

inline void Startup::Help(){
	std::cout << "Please use either --compress [filename] or --decompress [filename]" << std::endl;
	exit(-1);
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