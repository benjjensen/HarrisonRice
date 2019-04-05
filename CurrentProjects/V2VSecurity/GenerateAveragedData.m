function GenerateAveragedData(averageLength)
    addpath('Functions');

%     averageLength = 51;

    for testPoint = ['F' 'G' 'H' 'I' 'J']
        allCarriersFilename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged.mat', testPoint);
        middle32Filename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged-mid32.mat', testPoint);

        outputAllCarriers = sprintf('Data/CorrectlyAveragedData/test-point-%s_average%d.mat', testPoint, averageLength);
        outputMiddle32 = sprintf('Data/CorrectlyAveragedData/test-point-%s_average%d-mid32.mat', testPoint, averageLength);

        outputAllCarriersDb = sprintf('Data/CorrectlyAveragedData/test-point-%s_average%d-Db.mat', testPoint, averageLength);
        outputMiddle32Db = sprintf('Data/CorrectlyAveragedData/test-point-%s_average%d-mid32-Db.mat', testPoint, averageLength);


        load(allCarriersFilename); % pwelched
        pwelched = AveragePwelchedData(pwelched, averageLength);
        save(outputAllCarriers, 'pwelched');

        pwelchedDb = 10*log10(pwelched);
        save(outputAllCarriersDb, 'pwelchedDb');


        load(middle32Filename); % pwelched
        pwelched = AveragePwelchedData(pwelched, averageLength);
        save(outputMiddle32, 'pwelched');

        pwelchedDb = 10*log10(pwelched);
        save(outputMiddle32Db, 'pwelchedDb');
    end

end