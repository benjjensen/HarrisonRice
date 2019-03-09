function pwelchV2VFile(v2vMatFilename)
% Pwelch the V2V data in the given file.
% The data must have already been processed by the processV2VData function.
% The pwelched data is written to a new file.
% Author: Nathan Nelson
    framesPerCycle = 1000;
    samplesPerFrame = 14912;
    
    allCarriersOutputFilename = ...
        getAllCariersFilenameFromInputFilename(v2vMatFilename);
    middle32CarriersOutputFilename = ...
        getMiddle32FilenameFromInputFilename(v2vMatFilename);
    
    % Don't pwelch it if it already has been:
    if exist(allCarriersOutputFilename, 'file') == 2
        fprintf('%s has already been pwelched.\n', v2vMatFilename);
        return;
    end
    
    input = dsp.MatFileReader(v2vMatFilename,'VariableName','y', ...
        'SamplesPerFrame',framesPerCycle*samplesPerFrame);
    
    allCarriersOutput = ...
        matfile(allCarriersOutputFilename, 'Writable', true);
    middle32CarriersOutput = ...
        matfile(middle32CarriersOutputFilename, 'Writable', true);
    
    index = 1;
    count = 1;
    % Because the data set is probably too large to fit in memory, we're
    % going to pwelch it piece by piece.
    while ~isDone(input)
        disp(string(count));
        count = count + 1;
        
        y = input();
        inputData = y.real + 1i*y.imag;
        inputData = inputData';
        
        samples = size(inputData);
        samples = samples(2);
        frames = samples/samplesPerFrame;
        % If we're at the end of the file, just ignore the last fraction of
        % a frame.
        if mod(samples, samplesPerFrame) ~= 0
            frames = floor(frames);
            samples = samplesPerFrame * frames;
            inputData = inputData(1, samples);
        end
        
        % Pwelch the data.
        [allCarriers, middle32Carriers] = ...
            PwelchData(inputData, samplesPerFrame, false);
        
        % Write the pwelched data to the file.
        allCarriersOutput.pwelched(index:index+frames-1,1:64) = ...
            allCarriers;

        middle32CarriersOutput.pwelched(index:index+frames-1,1:32) = ...
            middle32Carriers;
        
        index = index + frames;
    end
    
    release(input);
end

function allCarriersFilename = getAllCariersFilenameFromInputFilename(inputFilename)
    L = strlength(inputFilename);
    allCarriersFilename = strcat(extractBefore(inputFilename, L-13), ...
        '_pwelched.mat');
end

function middle32CarriersFilename = getMiddle32FilenameFromInputFilename(inputFilename)
    L = strlength(inputFilename);
    middle32CarriersFilename = strcat(extractBefore(inputFilename, L-13), ...
        '_pwelched-mid32.mat');
end