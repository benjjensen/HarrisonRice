% clear all

data = readBatched_DAQshortSamples_host('/media/V2V/MR-Test-Wrong-Output_2018-10-01__16-40-08.dat');

figure(); clf
plot(1:10,data(1:10),'.-',...
    1:10,data(11:20),'-.',...
    1:10,data(21:30),'.-'); grid on;

Nfft = 1000;
PP = pwelch(data(1:500000)-8180.7,ones(1,Nfft),0,Nfft,'twosided');
FF = -0.5:1/Nfft:0.5-1/Nfft;
figure();
plot(FF*100,10*log10(fftshift(PP))); grid on;