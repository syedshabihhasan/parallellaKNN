function [ opStats ] = arrayStats( ipArray )
%ARRAYSTATS Extract the stats out of the input survey
%   Detailed explanation goes here
opStats = zeros(1,7);
opStats(1) = min(ipArray);
opStats(2) = max(ipArray);
opStats(3) = mean(ipArray);
opStats(4) = median(ipArray);
opStats(5) = std(ipArray);
opStats(6) = prctile(ipArray, 25);
opStats(7) = prctile(ipArray, 75);
end

