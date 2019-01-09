%%% mdrv2v
%%% the acquired data is in the variable data

x = data(1:500000);
F0 = 0.3;
h = firls(100,[0 0.1 0.12 0.5]*2,[1 1 0 0]);
n = (0:length(x)-1)';

y1 = x.*exp(-1i*2*pi*F0*n);
y2 = conv(y1,h);

Nfft = 64*100/20;

x1 = fft(y2(1:Nfft),Nfft);
x2 = 20*log10(abs(fftshift(x1)));
%%
FF = (-0.5:1/Nfft:0.5-1/Nfft)*100;
figure(1);
plot(FF,x2);
grid on;
ax = gca;
ax.XLim = [-25 25];
ax.YLim = [-10 90];

for idx = 2:3 %floor((length(data)-Nfft)/Nfft)
    x1 = fft(y2((idx-1)*Nfft+1:idx*Nfft),Nfft);
    ax.Children.YData = 20*log10(abs(fftshift(x1)));
    drawnow
end
