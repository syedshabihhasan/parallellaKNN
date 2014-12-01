function [ opStats ] = arrayStats( ipArray )
%ARRAYSTATS Extract the stats out of the input survey
%   Detailed explanation goes here
opStats = zeros(1,110);
opStats(1) = min(ipArray);
opStats(2) = max(ipArray);
opStats(3) = mean(ipArray);
opStats(4) = median(ipArray);
opStats(5) = std(ipArray);
opStats(6) = mad(ipArray);
opStats(7) = mad(ipArray,1);
opStats(8) = range(ipArray);
opStats(9) = geomean(ipArray);
opStats(10) = harmmean(ipArray);
P = 1:100;
opStats(11:110) = prctile(ipArray,P);
end

