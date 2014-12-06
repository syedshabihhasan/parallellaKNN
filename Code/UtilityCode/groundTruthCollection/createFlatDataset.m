function [ flatFile ] = createFlatDataset( actualDataset,savePath )
%CREATEFLATDATASET convert double based dataset to a single binary string
%   @author: Syed Shabih Hasan

if 1 == nargin
    savePath = '.';
end
flatFile = '';
[r,~] = size(actualDataset);
flatFileCell = cell(r,1);
parObj = parpool;
toSave = strcat(savePath,'/opBin.txt');
% f = fopen(toSave,'w');
parfor P=1:r
    %if 0 == mod(P,10000)
    %    disp(sprintf('P=%d',P));
    %end
    disp(sprintf('Parfor, P=%d',P));
    toConsider = actualDataset(P,2:end);
    temp = '';
    [~,n] = size(toConsider);
    for Q=1:n
        temp = strcat(temp,doub2bin(toConsider(Q)));
    end
%     fprintf(f,'%s\n',temp);
    flatFileCell{P} = temp;

end
% fclose(f);
delete(parObj);
save(sprintf('%s/cellFlat',savePath),'flatFileCell','-v7.3');
disp('Done with parallel job!');
flatFile = cell2mat(flatFileCell);
save(sprintf('%s/final_flat',savePath),'flatFile');
for P=1:r
    if 0 == mod(P,10000)
        disp(sprintf('I am at P=%d',P));
    end
    flatFile = [flatFile; flatFileCell{P}];
    if 0 == mod(P,100000)
       save(sprintf('%s/final_flat_%d',P),'flatFile','-v7.3');
       flatFile = [];
   end
end
end

