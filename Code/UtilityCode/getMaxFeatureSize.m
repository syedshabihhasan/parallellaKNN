function [ listOfFeatures ] = getMaxFeatureSize( listOfFeatures, fileList )
%GETMAXFEATURESIZE Summary of this function goes here
%   Detailed explanation goes here

featList = listOfFeatures.feature_names;

for P=1:length(fileList)
    disp(sprintf('Index: %d',P));
    try
        temp = load(fileList{P});
    catch err
        disp(sprintf('Error opening file, skipping, %s',fileList{P}));
        continue;
    end
    for Q=1:length(featList)
        fieldTest = eval(sprintf('isfield(temp,''%s'');',featList{Q}));
        if ~fieldTest
            continue;
        end
        cellTest = eval(sprintf('iscell(temp.%s);',featList{Q}));
        if ~cellTest
            nanTest = eval(sprintf('isnan(temp.%s);',featList{Q}));
        else
            nanTest = false;
        end
        if ~nanTest
            featureVals = eval(sprintf('listOfFeatures(strcmp(listOfFeatures.feature_names,''%s''),:)',featList{Q}));
            sizeVal = eval(sprintf('size(temp.%s)',featList{Q}));
            minSize = featureVals.feature_min_size;
            maxSize = featureVals.feature_max_size;
            if sizeVal(1) < minSize(1)
                minSize(1) = sizeVal(1);
            end
            if sizeVal(1) > maxSize(1)
                maxSize(1) = sizeVal(1);
            end
            if sizeVal(2) < minSize(2)
                minSize(2) = sizeVal(2);
            end
            if sizeVal(2) > maxSize(2)
                maxSize(2) = sizeVal(2);
            end
            featureVals.feature_min_size = minSize;
            featureVals.feature_max_size = maxSize;
            eval(sprintf('listOfFeatures(strcmp(listOfFeatures.feature_names,''%s''),:) = featureVals;',featList{Q}));
        else
            eval(sprintf('listOfFeatures.feature_nan_present(strcmp(listOfFeatures.feature_names,''%s'')) = true;',featList{Q}));
        end
    end
    disp(sprintf('Done with file, %s',fileList{P}));
end

end

