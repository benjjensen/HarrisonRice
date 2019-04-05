function convertDatFilesInDirectory(directoryPath)
% Convert the raw V2V data files in a folder into .mat files
% This function searches through a directory and converts all of the .dat
% files in that directory to .mat files. Files that have already been
% converted are automatically skipped.
%
% Author: Nathan Nelson
    pathLength = length(directoryPath);
    if directoryPath(pathLength) ~= '/'
        % Ensure that the path ends with a slash.
        directoryPath = strcat(directoryPath, '/');
    end
    
    searchPattern = strcat(directoryPath, '*.dat');
    % Get a list of all the files in the directory that have the .dat file
    % extension.
    datFiles = dir(searchPattern);
    
    numDatFiles = size(datFiles);
    % Convert eachfile in the directory.
    for i = 1:numDatFiles
        datFile = datFiles(i);
        datFilename = strcat(directory, datFile.name);
        if i ~= 1
           fprintf(' Done.\n');
        end
        fprintf('Working on %s (%d/%d)...', datFilename, i, numDatFiles);
        drawnow
        v2vDatFileToMatFile(datFilename);
    end
    fprintf(' Done.\nAll files converted./n');
end
