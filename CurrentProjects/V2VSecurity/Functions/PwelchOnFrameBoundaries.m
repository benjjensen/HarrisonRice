function PwelchOnFrameBoundaries(rawDataFilename, allCarriersFilename, ...
    middle32Filename, boundaries)

    if exist(allCarriersOutputFilename, 'file') == 2
        fprintf('%s has already been pwelched.\n', rawDataFilename);
        return;
    end
    
    input = matfile(rawDataFilename);
    
    frameCount = max(size(boundaries));
    
    allCarriers = zeros(frameCount, 64);
    middle32 = zeros(frameCount, 32);
    
    for idx = 1:frameCount
        [frameStart frameEnd] = boundaries(idx, :);
        data = input.y(frameStart:frameEnd);
        [allCarriers(idx, :) middle32(idx, :)] = PwelchData(data, frameStart-frameEnd+1, false);
    end
    
    pwelched = allCarriers;
    save(allCarriersFilename, 'pwelched');
    
    pwelchedDb = 10*log10(abs(pwelched));
    save(getDbFilename(allCarriersFilename), 'pwelchedDb');
    
    pwelched = middle32;
    save(middle32Filename, 'pwelched');
    
    pwelchedDB = 10*log10(abs(pwelched));
    save(getDbFilename(middle32Filename), 'pwelchedDb');
end

function filename = getDbFilename(filename)
    filename = filename(1:end-4);
    filename = strcat(filename, '-dB.mat');
end