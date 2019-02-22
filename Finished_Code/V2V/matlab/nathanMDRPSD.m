%%% nathanMDRPSD

Fs = 100;
F0 = 0.001;
N = 1000;

filename = '/media/V2V/PROVO-LOW_follow-straight_2019-01-29__15-49-37.mat';
m = matfile(filename);

temp = m.data(1:10000*N,1);
x = (temp - 8192)/8192;

Sx = pwelch(x,boxcar(N),0,N,'two-sided');
FF = -0.5:1/N:0.5-1/N;

figure(99);
plot(FF,10*log10(fftshift(Sx))); grid on;
xlabel('frequency (cycles/sample)');
ylabel('magnitude (dB)');
