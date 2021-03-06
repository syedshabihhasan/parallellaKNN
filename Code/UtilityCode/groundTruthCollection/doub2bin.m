function [ binVal ] = doub2bin( inputDouble )
%DOUB2BIN convert double to binary value ieee754 standard
%   @author: Syed Shabih Hasan
hexMap = {...
    '0','0000';...
    '1','0001';...
    '2','0010';...
    '3','0011';...
    '4','0100';...
    '5','0101';...
    '6','0110';...
    '7','0111';...
    '8','1000';...
    '9','1001';...
    'a','1010';...
    'b','1011';...
    'c','1100';...
    'd','1101';...
    'e','1110';...
    'f','1111';};

n2h = num2hex(inputDouble);
binVal = '';
n = length(n2h);
hexVals = hexMap(:,1);
for P=n:-2:1
    binVal = strcat(binVal,hexMap(strcmpi(hexVals,n2h(P-1)),2));
    binVal = strcat(binVal,hexMap(strcmpi(hexVals,n2h(P)),2));
end
binVal = binVal{:};
end

