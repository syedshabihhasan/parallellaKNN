function [ actualFilename ] = getActualFilename( filename )
%GETACTUALFILENAME Summary of this function goes here
%   @author: Syed Shabih Hasan

filename = strsplit(filename,'/');
filename = strsplit(filename{end},'.');
actualFilename = filename{1};

end

