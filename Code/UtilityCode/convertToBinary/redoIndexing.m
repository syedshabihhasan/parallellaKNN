function redoIndexing( binFilePath )
%REDOINDEXING Summary of this function goes here
%   @author: Syed Shabih Hasan

P=1;
f = fopen(binFilePath,'r+b');

for P=1:997747
    fwrite(f,P,'int',0,'l');
    fseek(f,1020,0);
end

fclose(f);

end

