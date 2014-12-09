function redoIndexing( binFilePath )
%REDOINDEXING Summary of this function goes here
%   @author: Syed Shabih Hasan

P=1;
f = fopen(binFilePath,'r+b');
if -1 == f
    disp('fid = -1, existing');
    exit;
end
for P=1:997747
    if 0 == mod(P,10000)
        disp(sprintf('P=%d',P));
    end
    try
        fwrite(f,P,'int',0,'l');
        fseek(f,1020,0);
    catch err
        disp(sprintf('An error occured at P = %d',P));
        err
    end
end

fclose(f);

end

