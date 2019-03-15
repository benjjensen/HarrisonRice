%% CombineAndProcessArraysTX2
% Created by Kalin Norman
% Should be a one time use file that converts all of the raw data into a
% usable form for all of the post-processing for the figures and other
% useful information. 

% Separate files for dB and linear data outputs are available. This one
% will output an array of numbers that are in linear

clear; close all;
%% Load Individual Arrays
for i = 1:26 % Load in data from Camacho's office
    eval(sprintf('load Data/tx2Data/tx2camacho_%d.mat;', i));
end
for i = 1:28 % Load in data from Chamber's office
    eval(sprintf('load Data/tx2Data/tx2chambers_%d.mat;', i));
end
for i = 1:65 % Load in data from Conference room
    eval(sprintf('load Data/tx2Data/tx2conference_%d.mat;', i));
end
for i = 1:23 % Load in data from Hallway
    eval(sprintf('load Data/tx2Data/tx2hallway_%d.mat;', i));
end
for i = 1:30 % Load in data from Harrison's office
    eval(sprintf('load Data/tx2Data/tx2harrison_%d.mat;', i));
end
for i = 1:32 % Load in data from Smalley's office
    eval(sprintf('load Data/tx2Data/tx2smalley_%d.mat;', i));
end

%% Combine Into One Array Per Room

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section takes the individual arrays generated from data
% collection and first turns each section or room into a complete
% three dimensional array that is oriented such that it can be used
% for the generation of a heatmap (co-ordinate (1,1) is in the top
% left corner of the array). After each section, or room, has been
% oriented and combined, those arrays are then inserted into one large
% array that represents the entire section where our data was
% collected. Points where we do not have data are represented by NaN's
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

%% Pwelch Data And Leave In Linear

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section takes the arrays that are created in the above section and 
% runs each of them through a pwelch function in order to obtain 
% understandable data from what was originally gathered.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
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
            YY = pwelch(tx2hallway_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2hallway_pwelch(:,loops,count) = fftshift(YY);
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
            YY = pwelch(tx2camacho_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2camacho_pwelch(:,loops,count) = fftshift(YY);
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
            YY = pwelch(tx2chambers_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2chambers_pwelch(:,loops,count) = fftshift(YY);
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
            YY = pwelch(tx2conference_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2conference_pwelch(:,loops,count) = fftshift(YY);
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
            YY = pwelch(tx2harrison_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2harrison_pwelch(:,loops,count) = fftshift(YY);
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
            YY = pwelch(tx2smalley_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2smalley_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end

%% Piece Together Into One Array

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file takes the pwelch data arrays and inserts them in the proper
% locations with respect to one another into one array that represents the
% complete area over which the data was collected
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

tx2_linear = nan(128,91,345);
% Insert Conference room
for temp = 1:65
    for loops = 1:61
        obj = 'tx2conference_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,temp,loops) = eval(obj);
    end
end
% Insert Harrison's office
for temp = 1:30
    for loops = 1:34
        obj = 'tx2harrison_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,35+temp,64+loops) = eval(obj);
    end
end
% Insert Smalley's office
for temp = 1:32
    for loops = 1:26
        obj = 'tx2smalley_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,33+temp,122+loops) = eval(obj);
    end
end
% Insert Camacho's office
for temp = 1:32
    for loops = 1:26
        obj = 'tx2camacho_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,33+temp,153+loops) = eval(obj);
    end
end
% Insert Chamber's office
for temp = 1:32
    for loops = 1:28
        obj = 'tx2chambers_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,33+temp,212+loops) = eval(obj);
    end
end
% Insert Hallway
for temp = 1:23
    for loops = 1:300
        obj = 'tx2hallway_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_linear(:,68+temp,42+loops) = eval(obj);
    end
end

%% Separate Signal and Noise Carriers

tx2_signal_linear(:,:,:) = tx2_linear(1:2:end,:,:);
tx2_noise_linear(:,:,:) = tx2_linear(2:2:end,:,:);

tx2_signal_lin_gc(:,:,:) = tx2_signal_linear(11:55,:,:);
tx2_noise_lin_gc(:,:,:) = tx2_noise_linear(10:54,:,:);

save Data/tx2_linear.mat tx2_linear;
save Data/tx2_signal_linear.mat tx2_signal_linear;
save Data/tx2_noise_linear.mat tx2_noise_linear;
save Data/tx2_signal_lin_gc.mat tx2_signal_lin_gc;
save Data/tx2_noise_lin_gc.mat tx2_noise_lin_gc;