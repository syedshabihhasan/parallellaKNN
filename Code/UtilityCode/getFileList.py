import sys;
import os;
import fnmatch;

def main(toLookIn, fileExt, toOutputIn):
	f = open(toOutputIn,'w');
	for root, subdirs, filenames in os.walk(toLookIn):
		for filen in fnmatch.filter(filenames,'*.'+fileExt):
			f.write(os.path.join(root,filen)+'\n');
	f.close();
if __name__ == "__main__":
	main(sys.argv[1],sys.argv[2],sys.argv[3]);
