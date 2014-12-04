function [ flatFile ] = createFlatDataset( actualDataset,savePath )
%CREATEFLATDATASET convert double based dataset to a single binary string
%   @author: Syed Shabih Hasan

if 1 == nargin
    savePath = '.';
end
flatFile = [];
[r,~] = size(actualDataset);

for P=1:r
    if 0 == mod(P,10000)
        disp(sprintf('I am at P=%d',P));
    end
    toConsider = actualDataset(P,2:end);
    temp = '';
    [~,n] = size(toConsider);
    for Q=1:n
        temp = strcat(temp,doub2bin(toConsider(Q)));
    end
    flatFile = [flatFile; temp;];
    if 0 == mod(P,100000)
        save(sprintf('%s/final_flat_%d',P),'flatFile');
        flatFile = [];
    end
end

end

