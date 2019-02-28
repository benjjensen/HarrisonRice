function processV2VDataInDirectory(directoryPath, includeTimestamp)
    pathLength = length(directoryPath);
    if directoryPath(pathLength) ~= '/'
        directoryPath = strcat(directoryPath, '/');
    end
    
    searchPattern = strcat(directoryPath, '*.mat');
    matFiles = dir(searchPattern);
    
    numMatFiles = size(matFiles);
    
    matFilenames = strings(numMatFiles(1), 1);
    
    correctSize = 0;
    for i = 1:numMatFiles
        matFile = matFiles(i);
        matFilename = strcat(directoryPath, matFile.name);
        listOfVariables = who('-file', matFilename);
        if ismember('data', listOfVariables)
            correctSize = correctSize + 1;
            matFilenames(correctSize) = matFilename;
        end
    end
    
    if correctSize == 0
        disp('No V2V data files found in this directory.\n');
        return;
    end
    
    matFilenames = matFilenames(1:correctSize);
    
    numMatFiles = size(matFilenames);
    for i = 1:numMatFiles
        matFilename = matFilenames(i);
        if i ~= 1
           fprintf(' Done.\n');
        end
        fprintf('Processing %s (%d/%d)...', matFilename, i, numMatFiles);
        drawnow;
        inFilename = matFilename;
        outFilename = getProcessedFilenameFromRawFilename(matFilename, includeTimestamp);
        processV2VData(inFilename, outFilename);
    end
    fprintf(' Done.\nAll files processed.\n');
end

function processedFilename = getProcessedFilenameFromRawFilename(rawFilename, includeTimestamp)
    L = strlength(rawFilename);
    if ~includeTimestamp
        processedFilename = strcat(extractBefore(rawFilename, L-24), '_processed.mat');
    else
        processedFilename = strcat(extractBefore(rawFilename, L-24), '_processed', ...
            extractAfter(rawFilename, L-25));
    end
end