import sys;

def combineBinFiles(combFile, toSaveFile):
    f = open(combFile,'r');
    allCombs = f.read();
    f.close();
    allCombs = allCombs.split('\n');
    try:
        allCombs.remove('');
    except ValueError:
        pass;
    k=1;
    f = open(toSaveFile,'w');
    toWrite = '';
    for fileLine in allCombs:
        if 10000 == k:
            f.write(toWrite);
            toWrite = '';
            k = 0;
        toWorkWith = fileLine.split(',')[0];
        f_bin = open(toWorkWith,'r');
        toWrite+=f_bin.read();
        f_bin.close();
        k+=1;
    f.write(toWrite);
    f.close();

if __name__ == "__main__":
    combineBinFiles(sys.argv[1],sys.argv[2]);