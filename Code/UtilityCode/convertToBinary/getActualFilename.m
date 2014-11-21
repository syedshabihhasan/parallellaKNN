function [ actualFilename ] = getActualFilename( filename )
%GETACTUALFILENAME Summary of this function goes here
%   Detailed explanation goes here

filename = strsplit(filename,'/');
filename = strsplit(filename{end},'.');
actualFilename = filename{1};

end

