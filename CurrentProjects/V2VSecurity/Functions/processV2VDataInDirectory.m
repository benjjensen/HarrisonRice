function processV2VDataInDirectory(directoryPath, includeTimestamp)
% Run processV2VData on all V2V data files in a directory.
% Author: Nathan Nelson
    pathLength = length(directoryPath);
    if directoryPath(pathLength) ~= '/'
        directoryPath = strcat(directoryPath, '/');
    end
    
    searchPattern = strcat(directoryPath, '*.mat');
    % Get a list of all .mat files in the given directory.
    matFiles = dir(searchPattern);
    
    numMatFiles = size(matFiles);
    
    matFilenames = strings(numMatFiles(1), 1);
    
    correctSize = 0;
    % Find only the .mat files that have variables named data; these are
    % the V2V files.
    for i = 1:numMatFiles
        matFile = matFiles(i);
        matFilename = strcat(directoryPath, matFile.name);
        listOfVariables = who('-file', matFilename);
        if ismember('data', listOfVariables)
            correctSize = correctSize + 1;
            matFilenames(correctSize) = matFilename;
        end
    end
    
    % If none were found, print a message and return.
    if correctSize == 0
        disp('No V2V data files found in this directory.\n');
        return;
    end
    
    matFilenames = matFilenames(1:correctSize);
    
    numMatFiles = size(matFilenames);
    % For each V2V .mat file, process it.
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