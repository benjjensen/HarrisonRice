clear;
load ('Data_for_GenLinearDataArrays/tx2_camacho_arrays.mat');
load ('Data_for_GenLinearDataArrays/tx2_chambers_arrays.mat');
load ('Data_for_GenLinearDataArrays/tx2_conference_arrays.mat');
load ('Data_for_GenLinearDataArrays/tx2_hallway_arrays1.mat');
load ('Data_for_GenLinearDataArrays/tx2_hallway_arrays2.mat');
load ('Data_for_GenLinearDataArrays/tx2_harrison_arrays.mat');
load ('Data_for_GenLinearDataArrays/tx2_smalley_arrays.mat');

%%% CreateDataArray %%%
for i = 1:1
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % This file takes the individual arrays generated from data
    % collection and first turns each section or room into a complete
    % three dimensional array that is oriented such that it can be used
    % for the generation of a heatmap (co-ordinate (1,1) is in the top
    % left corner of the array). After each section, or room, has been
    % oriented and combined, those arrays are then inserted into one large
    % array that represents the entire section where our data was
    % collected. Points where we do not have data are represented by NaN's
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Second Transmitter Location
    % Desired format is (area)_data(2048,y-axis,x-axis)
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % Hallway
    numbarrays = 23;
    numbloops = 300;
    tx2hallway_data = nan(2048,numbarrays,numbloops);
    
    for numbtests = 1:numbarrays
        obj = 'tx2hallway_' + string(numbtests) + '(:,:)';
        tx2hallway_data(:,numbtests,:) = eval(obj);
    end
    
    % Camacho's office
    numbarrays = 26;
    numbloops = 32;
    tx2camacho_data = nan(2048,numbloops,numbarrays);
    
    for numbtests = 1:numbarrays
        obj = 'tx2camacho_' + string(numbtests) + '(:,:)';
        tx2camacho_data(:,:,numbtests) = eval(obj);
    end
    
    % Chambers' office
    numbarrays = 28;
    numbloops = 32;
    tx2chambers_data = nan(2048,numbloops,numbarrays);
    
    for numbtests = 1:numbarrays - 2
        obj = 'tx2chambers_' + string(numbtests) + '(:,:)';
        tx2chambers_data(:,:,numbtests) = eval(obj);
    end
    
    for numbtests = numbarrays - 1:numbarrays
        obj = 'tx2chambers_' + string(numbtests) + '(:,:)';
        tx2chambers_data(:,1:29,numbtests) = eval(obj);
    end
    
    % Conference Room
    numbarrays = 65;
    numbloops = 61;
    tx2conference_data = nan(2048,numbarrays,numbloops);
    
    for numbtests = 1:4
        for numbcorrections = 1:30
            obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
            tx2conference_data(:,numbtests,31 + numbcorrections) = eval(obj);
        end
    end
    
    for numbtests = 5:7
        for numbcorrections = 1:58
            obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
            tx2conference_data(:,numbtests,3 + numbcorrections) = eval(obj);
        end
    end
    
    for numbtests = 8:62
        for numbcorrections = 1:61
            obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
            tx2conference_data(:,numbtests,numbcorrections) = eval(obj);
        end
    end
    
    for numbtests = 63:65
        for numbcorrections = 1:55
            obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
            tx2conference_data(:,numbtests,3 + numbcorrections) = eval(obj);
        end
    end
    
    % Harrison's office
    numbarrays = 30;
    numbloops = 34;
    tx2harrison_data = nan(2048,numbarrays,numbloops);
    
    for numbtests = 1:numbarrays
        obj = 'tx2harrison_' + string(numbtests) + '(:,:)';
        tx2harrison_data(:,numbtests,:) = eval(obj);
    end
    
    % Smalley's office
    numbarrays = 32;
    numbloops = 26;
    tx2smalley_data = nan(2048,numbarrays,numbloops);
    
    for numbtests = 1:numbarrays
        obj = 'tx2smalley_' + string(numbtests) + '(:,:)';
        tx2smalley_data(:,numbtests,:) = eval(obj);
    end
    
end
%%% PwelchData %%%
for i = 1:1
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % This file takes the arrays that are created in CreateDataArray and runs
    % each of them through a pwelch function in order to obtain understandable
    % data from what was originally gathered. This pwelch can be graphed in
    % order to better understand the strengths of the signals
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    Nfft = 2*64;
    FF = -0.5:1/Nfft:0.5-1/Nfft;
    FF = 20*FF;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Second Transmitter Location
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % Hallway
    clear nanarray;
    nanarray = isnan(tx2hallway_data);
    xlength = 300;
    ylength = 23;
    tx2hallway_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2hallway_pwelch(:,loops,count) = pwelch(tx2hallway_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
    
    % Camacho's Office
    clear nanarray;
    nanarray = isnan(tx2camacho_data);
    xlength = 26;
    ylength = 32;
    tx2camacho_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2camacho_pwelch(:,loops,count) = pwelch(tx2camacho_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
    
    % Chamber's Office
    clear nanarray;
    nanarray = isnan(tx2chambers_data);
    xlength = 28;
    ylength = 32;
    tx2chambers_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2chambers_pwelch(:,loops,count) = pwelch(tx2chambers_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
    
    % Conference Room
    clear nanarray;
    nanarray = isnan(tx2conference_data);
    xlength = 61;
    ylength = 65;
    tx2conference_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2conference_pwelch(:,loops,count) = pwelch(tx2conference_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
    
    % Harrison's Office
    clear nanarray;
    nanarray = isnan(tx2harrison_data);
    xlength = 34;
    ylength = 30;
    tx2harrison_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2harrison_pwelch(:,loops,count) = pwelch(tx2harrison_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
    
    % Smalley's Office
    clear nanarray;
    nanarray = isnan(tx2smalley_data);
    xlength = 26;
    ylength = 32;
    tx2smalley_pwelch = nan(128,ylength,xlength);
    
    for count = 1:xlength
        for loops = 1:ylength
            if nanarray(:,loops,count) == 1
                
            else
                tx2smalley_pwelch(:,loops,count) = pwelch(tx2smalley_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            end
        end
    end
    
end
%%% PieceTogetherArray %%%
for i = 1:1
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % This file takes the pwelch data arrays and inserts them in the proper
    % locations with respect to one another into one array that represents the
    % complete area over which the data was collected
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Second Transmitter Location
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    tx2_pwelch = nan(128,90,345);
    
    for temp = 1:65
        for loops = 1:61
            obj = 'tx2conference_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,temp,loops) = eval(obj);
        end
    end
    
    for temp = 1:30
        for loops = 1:34
            obj = 'tx2harrison_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,35+temp,64+loops) = eval(obj);
        end
    end
    
    for temp = 1:32
        for loops = 1:26
            obj = 'tx2smalley_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,33+temp,122+loops) = eval(obj);
        end
    end
    
    for temp = 1:32
        for loops = 1:26
            obj = 'tx2camacho_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,33+temp,153+loops) = eval(obj);
        end
    end
    
    for temp = 1:32
        for loops = 1:28
            obj = 'tx2chambers_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,33+temp,212+loops) = eval(obj);
        end
    end
    
    for temp = 1:23
        for loops = 1:300
            obj = 'tx2hallway_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
            tx2_pwelch(:,68+temp,42+loops) = eval(obj);
        end
    end
    
    % save('tx2_pwelch.mat','tx2_pwelch');
end
%%% SeparateSignals %%%
for i = 1:1
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % This file is used to separate the pwelch data into the noise floor and
    % signal carriers, after which the difference (in dB) between the two is
    % found and a new array is created that represents the number of carriers
    % at each given location that are above a specified number of dB from the
    % noise floor.
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Second Transmitter Location
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    linear_signal = nan(64,90,345);
    linear_noisefloor = nan(64,90,345);
    
    for temp = 1:90
        for loops = 1:345
            linear_signal(:,temp,loops) = tx2_pwelch(1:2:end,temp,loops);
            linear_noisefloor(:,temp,loops) = tx2_pwelch(2:2:end,temp,loops);
            
        end
    end
    
    save('linear_signal.mat','linear_signal');
    save('linear_noisefloor.mat','linear_noisefloor');
end
%%% AverageNoisefloor %%%
for i = 1:1
    for x = 1:64
        for y = 1:90
            for z = 1:345
                linear_noisefloor_avg(x,y,z) = sum(linear_noisefloor(:,y,z));
            end
        end
    end
    linear_noisefloor_avg = linear_noisefloor_avg / 64;
    save('linear_noisefloor_avg.mat','linear_noisefloor_avg');
end

%% Truncate arrays
load linear_signal.mat;
load linear_noisefloor.mat;
truncated_linear_signal = linear_signal(11:55,:,:);
truncated_linear_noise = linear_noisefloor(10:54,:,:);

save('truncated_linear_signal.mat','truncated_linear_signal');
save('truncated_linear_noise.mat','truncated_linear_noise');

for x = 1:45
    for y = 1:90
        for z = 1:345
            truncated_linear_noise_avg(x,y,z) = sum(truncated_linear_noise(:,y,z));
        end
    end
end
truncated_linear_noise_avg = truncated_linear_noise_avg / 45;
save('truncated_linear_noise_avg.mat','truncated_linear_noise_avg');

count = 0;
avg = 0;
for x = 1:1
    for y = 1:90
        for z = 1:345
            if isnan(truncated_linear_noise_avg(x,y,z))
            else
                count = count + 1;
                avg = avg + truncated_linear_noise_avg(x,y,z);
            end
        end
    end
end
avg = avg / count;