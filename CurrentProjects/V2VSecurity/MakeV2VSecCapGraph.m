function MakeV2VSecCapGraph(averageLength, snr)
    addpath(genpath('Functions'))

    if averageLength ~= 1
        if ~exist(sprintf('Data/CorrectlyAveragedData/test-point-F_average%d-mid32.mat', averageLength), 'file')
            GenerateAveragedData(averageLength);
        end

        load(sprintf('Data/CorrectlyAveragedData/test-point-F_average%d-mid32.mat', averageLength));
        foxtrot = pwelched;
        load(sprintf('Data/CorrectlyAveragedData/test-point-G_average%d-mid32.mat', averageLength));
        golf = pwelched;
        load(sprintf('Data/CorrectlyAveragedData/test-point-H_average%d-mid32.mat', averageLength));
        hotel = pwelched;
        load(sprintf('Data/CorrectlyAveragedData/test-point-I_average%d-mid32.mat', averageLength));
        india = pwelched;
        load(sprintf('Data/CorrectlyAveragedData/test-point-J_average%d-mid32.mat', averageLength));
        juliet = pwelched;
        clear pwelched;
    else
        load('Data/CorrectlyAveragedData/test-point-F_jagged-mid32.mat');
        foxtrot = pwelched;
        load('Data/CorrectlyAveragedData/test-point-G_jagged-mid32.mat');
        golf = pwelched;
        load('Data/CorrectlyAveragedData/test-point-H_jagged-mid32.mat');
        hotel = pwelched;
        load('Data/CorrectlyAveragedData/test-point-I_jagged-mid32.mat');
        india = pwelched;
        load('Data/CorrectlyAveragedData/test-point-J_jagged-mid32.mat');
        juliet = pwelched;
        clear pwelched;
    end
    
    
    fox_max = max(max(foxtrot));
    golf_max = max(max(golf));
    hotel_max = max(max(hotel));
    india_max = max(max(india));
    juliet_max = max(max(juliet));
    normalize_max = sqrt(max([fox_max,golf_max,hotel_max,india_max,juliet_max]));
    foxtrot = (sqrt(foxtrot)/normalize_max).^2;
    golf = (sqrt(golf)/normalize_max).^2;
    hotel = (sqrt(hotel)/normalize_max).^2;
    india = (sqrt(india)/normalize_max).^2;
    juliet = (sqrt(juliet)/normalize_max).^2;


    
    sizeInfo = size(foxtrot);
    numLocations= max(size(foxtrot));
    numCarriers = sizeInfo(2);
    
    clear sizeInfo;

    foxtrotSum = 0;
    golfSum = 0;
    hotelSum = 0;
    indiaSum = 0;
    julietSum = 0;
    count = 0;
    
    for location = 1+floor((averageLength-1)/2):numLocations-ceil((averageLength-1)/2)
        for carrier = 1:numCarriers
            foxtrotSum = foxtrotSum + foxtrot(location, carrier);
            golfSum = golfSum + golf(location, carrier);
            hotelSum = hotelSum + hotel(location, carrier);
            indiaSum = indiaSum + india(location, carrier);
            julietSum = julietSum + juliet(location, carrier);
            count = count + 1;
        end
    end
    foxtrotAverage = 10*log10(foxtrotSum / count) + snr;
    golfAverage = 10*log10(golfSum / count) + snr;
    hotelAverage = 10*log10(hotelSum / count) + snr;
    indiaAverage = 10*log10(indiaSum / count) + snr;
    julietAverage = 10*log10(julietSum / count) + snr;
    
    save averageSNR.mat foxtrotAverage golfAverage hotelAverage indiaAverage julietAverage
    
    
    
    golf_sec_cap = zeros(numLocations,1);
    hotel_sec_cap = zeros(numLocations,1);
    india_sec_cap = zeros(numLocations,1);
    juliet_sec_cap = zeros(numLocations,1);
    
    linearSNR = 10^(snr/10); % convert SNR to linear
    
    % Capacity
    [~, foxtrot_carrier_capacity] = gaussian_capacity(foxtrot,linearSNR);
    [~, golf_carrier_capacity] = gaussian_capacity(golf,linearSNR);
    [~, hotel_carrier_capacity] = gaussian_capacity(hotel,linearSNR);
    [~, india_carrier_capacity] = gaussian_capacity(india,linearSNR);
    [~, juliet_carrier_capacity] = gaussian_capacity(juliet,linearSNR);

    % Secrecy Capacity
    for x = 1 : numLocations
        for carrier = 1:32
            g_sec = foxtrot_carrier_capacity(x,carrier) - ...
                golf_carrier_capacity(x,carrier);
            if g_sec < 0
                g_sec = 0;
            end
            golf_sec_cap(x) = golf_sec_cap(x) + g_sec;

            h_sec = foxtrot_carrier_capacity(x,carrier) - ...
                hotel_carrier_capacity(x,carrier);
            if h_sec < 0
                h_sec = 0;
            end
            hotel_sec_cap(x) = hotel_sec_cap(x) + h_sec;

            i_sec = foxtrot_carrier_capacity(x,carrier) - ...
                india_carrier_capacity(x,carrier);
            if i_sec < 0
                i_sec = 0;
            end
            india_sec_cap(x) = india_sec_cap(x) + i_sec;

            j_sec = foxtrot_carrier_capacity(x,carrier) - ...
                juliet_carrier_capacity(x,carrier);
            if j_sec < 0
                j_sec = 0;
            end
            juliet_sec_cap(x) = juliet_sec_cap(x) + j_sec;
        end
    end

    figure(averageLength)
    hold on
    plot(golf_sec_cap);
    plot(hotel_sec_cap);
    plot(india_sec_cap);
    plot(juliet_sec_cap);
    ylabel('Bits per channel use')
    xlabel('location');
    legend('Eve1 Sec Cap','Eve2 Sec Cap','Eve3 Sec Cap','Eve4 Sec Cap');
    title(sprintf('SNR = %f dB\nAverage Length = %d', snr, averageLength));
    hold off

    % save V2VSecrecyCapacity.mat golf_sec_cap hotel_sec_cap india_sec_cap juliet_sec_cap snr averageLength
end




