%%% PwelchData %%%

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
% First Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Hallway
clear nanarray;
nanarray = isnan(hallway_data);
xlength = 300;
ylength = 23;
hallway_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(hallway_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            hallway_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


% Camacho's Office
clear nanarray;
nanarray = isnan(camacho_data);
xlength = 26;
ylength = 32;
camacho_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(camacho_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            camacho_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


% Chamber's Office
clear nanarray;
nanarray = isnan(chambers_data);
xlength = 28;
ylength = 32;
chambers_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(chambers_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            chambers_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


% Conference Room
clear nanarray;
nanarray = isnan(conference_data);
xlength = 61;
ylength = 65;
conference_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(conference_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            conference_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


% Harrison's Office
clear nanarray;
nanarray = isnan(harrison_data);
xlength = 33;
ylength = 29;
harrison_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(harrison_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            harrison_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


% Smalley's Office
clear nanarray;
nanarray = isnan(smalley_data);
xlength = 26;
ylength = 31;
smalley_pwelch = nan(128,ylength,xlength);

for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(smalley_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            smalley_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


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
            YY = pwelch(tx2hallway_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            tx2hallway_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
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
            tx2camacho_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
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
            tx2chambers_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
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
            tx2conference_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
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
            tx2harrison_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
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
            tx2smalley_pwelch(:,loops,count) = 10*log10(abs(fftshift(YY)));
        end
    end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Option to save the files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% save('hallway_pwelch.mat','hallway_pwelch');
% save('camacho_pwelch.mat','camacho_pwelch');
% save('chambers_pwelch.mat','chambers_pwelch');
% save('conference_pwelch.mat','conference_pwelch');
% save('harrison_pwelch.mat','harrison_pwelch');
% save('smalley_pwelch.mat','smalley_pwelch');
% save('tx2hallway_pwelch.mat','tx2hallway_pwelch');
% save('tx2camacho_pwelch.mat','tx2camacho_pwelch');
% save('tx2chambers_pwelch.mat','tx2chambers_pwelch');
% save('tx2conference_pwelch.mat','tx2conference_pwelch');
% save('tx2harrison_pwelch.mat','tx2harrison_pwelch');
% save('tx2smalley_pwelch.mat','tx2smalley_pwelch');
