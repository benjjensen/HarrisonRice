function PwelchOnFrameBoundaries(rawDataFilename, allCarriersFilename, ...
    middle32Filename, boundaries)

    if exist(allCarriersFilename, 'file') == 2
        fprintf('%s has already been pwelched.\n', rawDataFilename);
        return;
    end
    
    input = matfile(rawDataFilename);
    
    frameCount = max(size(boundaries));
    
    allCarriers = zeros(frameCount, 64);
    middle32 = zeros(frameCount, 32);
    
    for idx = 1:frameCount
        frameStart = boundaries(idx,1); 
        frameEnd = boundaries(idx, 2);
        data = input.y(frameStart:frameEnd,:);
        data = data';
        [allCarriers(idx, :) middle32(idx, :)] = PwelchData(data, frameEnd-frameStart+1, false);
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