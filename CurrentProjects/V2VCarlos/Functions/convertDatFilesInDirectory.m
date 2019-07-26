 function convertDatFilesInDirectory(directoryPath, sampleRate)
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
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Option 1 - Read in 'some' of the dat files from the directory
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    fileEnding = '*2019-07-26*.dat'; % Change this to narrow selection
    searchPattern = strcat(directoryPath, fileEnding);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Option 2 - Read in all dat files from the directory
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
%     searchPattern = strcat(directoryPath, '*.dat');

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    % Get a list of all the files in the directory that have the .dat file
    % extension.
    datFiles = dir(searchPattern);
    
    [numDatFiles, ~] = size(datFiles);
    % Convert eachfile in the directory.
    for i = 1:numDatFiles
        datFile = datFiles(i);
        datFilename = strcat(directoryPath, datFile.name);
        if i ~= 1
           fprintf(' Done at %s\n', datetime('now','Format','HH:mm:ss'));
        end
        fprintf('Working on %s (%d/%d)...', datFilename, i, numDatFiles);
        drawnow
        v2vDatFileToMatFile(datFilename,sampleRate);
    end
    fprintf(' Done at %s.\nAll files converted.\n', datetime('now','Format','HH:mm:ss'));
end