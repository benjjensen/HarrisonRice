clear; close all;

load Data/camacho_array;
load Data/chambers_array;
load Data/conference_array;
load Data/harrison_array;
load Data/smalley_array;

Nfft = 2*64;
numFrames = 4000;
for loops = 1:numFrames
    % Camacho
    a = pwelch(camacho_array(:,loops),boxcar(Nfft),0,Nfft,'twosided');
    camacho_pwelch(:,loops) = fftshift(a);
    camacho_dB(:,loops) = 10*log10(abs(fftshift(a)));
    % Chambers
    b = pwelch(chambers_array(:,loops),boxcar(Nfft),0,Nfft,'twosided');
    chambers_pwelch(:,loops) = fftshift(b);
    chambers_dB(:,loops) = 10*log10(abs(fftshift(b)));
    % Conference
    c = pwelch(conference_array(:,loops),boxcar(Nfft),0,Nfft,'twosided');
    conference_pwelch(:,loops) = fftshift(c);
    conference_dB(:,loops) = 10*log10(abs(fftshift(c)));
    % Harrison
    d = pwelch(harrison_array(:,loops),boxcar(Nfft),0,Nfft,'twosided');
    harrison_pwelch(:,loops) = fftshift(d);
    harrison_dB(:,loops) = 10*log10(abs(fftshift(d)));
    % Smalley
    e = pwelch(smalley_array(:,loops),boxcar(Nfft),0,Nfft,'twosided');
    smalley_pwelch(:,loops) = fftshift(e);
    smalley_dB(:,loops) = 10*log10(abs(fftshift(e)));
end

numCarriers = 128;
cam = zeros(numCarriers,1);
cha = zeros(numCarriers,1);
con = zeros(numCarriers,1);
har = zeros(numCarriers,1);
sma = zeros(numCarriers,1);

for loops = 1:numCarriers
    cam(loops,1) = sum(camacho_dB(loops,:));
    cha(loops,1) = sum(chambers_dB(loops,:));
    con(loops,1) = sum(conference_dB(loops,:));
    har(loops,1) = sum(harrison_dB(loops,:));
    sma(loops,1) = sum(smalley_dB(loops,:));
end

figure();
bar(cam); title('cam');
figure();
bar(cha); title('cha');
figure();
bar(con); title('con');
figure();
bar(har); title('har');
figure();
bar(sma); title('sma');

camacho_pwelch = camacho_pwelch(21:108,:);
chambers_pwelch = chambers_pwelch(21:108,:);
conference_pwelch = conference_pwelch(21:108,:);
harrison_pwelch = harrison_pwelch(21:108,:);
smalley_pwelch = smalley_pwelch(21:108,:);
% 
% save Data/camacho_pwelch camacho_pwelch;
% save Data/chambers_pwelch chambers_pwelch;
% save Data/conference_pwelch conference_pwelch;
% save Data/harrison_pwelch harrison_pwelch;
% save Data/smalley_pwelch smalley_pwelch;
