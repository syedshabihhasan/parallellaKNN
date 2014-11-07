'''
	@author: syedshabihhasan

	Usage: 
	python getFileList.py <folder to look into> <extension to find without '.'> <path to output file>
	
	For example, if we want to find all files with extension '.ext' 
	(eg. a.ext, b.ext, somename.ext etc.) in the folder foo 
	(located at /Documents/foo/) and output the list of files to 
	op.txt
	python getFileList.py /Documents/foo/ ext ./op.txt
'''
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
