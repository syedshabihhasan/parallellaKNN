function [ nearestNeighbors ] = doKNNSearch( flatFilePath, K )
%DOKNNSEARCH Summary of this function goes here
%   @author: Syed Shabih Hasan

load(flatFilePath);
nearestNeighbors = knnsearch(flatFile,flatFilePath,'K',K,...
                                        'Distance','hamming');

end

