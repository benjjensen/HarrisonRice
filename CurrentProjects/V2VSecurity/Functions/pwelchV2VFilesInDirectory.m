function pwelchV2VFilesInDirectory(directoryPath)
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
        if ismember('y', listOfVariables)
            correctSize = correctSize + 1;
            matFilenames(correctSize) = matFilename;
        end
    end
    
    if correctSize == 0
        disp('No processed V2V data files found in this directory.\n');
        return;
    end
    
    matFilenames = matFilenames(1:correctSize);
    
    numMatFiles = size(matFilenames);
    for i = 1:numMatFiles
        matFilename = matFilenames(i);
        if i ~= 1
           fprintf('Done.\n');
        end
        fprintf('Pwelching %s (%d/%d)...\n', matFilename, i, numMatFiles);
        drawnow;
        pwelchV2VFile(matFilename);
    end
    fprintf('Done.\nAll files pwelched.\n');
end