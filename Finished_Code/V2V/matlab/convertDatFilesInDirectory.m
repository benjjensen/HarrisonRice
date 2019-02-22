function convertDatFilesInDirectory(directoryPath)
    pathLength = length(directoryPath);
    if directoryPath(pathLength) ~= '/'
        directoryPath = strcat(directoryPath, '/');
    end
    
    searchPattern = strcat(directoryPath, '*.dat');
    datFiles = dir(searchPattern);
    
    numDatFiles = size(datFiles);
    for i = 1:numDatFiles;
        datFile = datFiles(i);
        datFilename = strcat(directory, datFile.name);
        if i ~= 1
           fprintf(' Done.\n');
        end
        fprintf('Working on %s (%d/%d)...', datFilename, i, numDatFiles);
        v2vDatFileToMatFile(datFilename);
    end
    fprintf(' Done.\nAll files converted.');
end