function [ flatFile ] = createFlatDataset( actualDataset )
%CREATEFLATDATASET convert double based dataset to a single binary string
%   @author: Syed Shabih Hasan

flatFile = [];
[r,~] = size(actualDataset);

for P=1:r
    toConsider = actualDataset(P,2:end);
    temp = '';
    [~,n] = size(toConsider);
    for Q=1:n
        temp = strcat(temp,doub2bin(toConsider(Q)));
    end
    flatFile = [flatFile; temp;];
end

end

