%%% testMDRTests

fileName = 'mdrTest3.mat';
m = matfile(fileName);
yy = m.y(1:128,1);
yy = conj(yy);
Nfft = 128;
FF = -0.5:1/Nfft:0.5-1/Nfft;
YY = fft(yy(1:Nfft),Nfft);
YY2 = abs(fftshift(YY));
figure(901);
stem(FF(2:2:end),YY2(2:2:end),'Marker','none');
grid on;
ax = gca;
ax.YLim = [0 0.2];

for idx = 2:1000
    yy = m.y((idx-1)*128+1:idx*128,1);
    yy = conj(yy);
    YY = fft(yy(1:Nfft),Nfft);
    YY2 = abs(fftshift(YY));
    ax.Children.YData = YY2(2:2:end);
    drawnow;
end


return
figure(900);
stem(FF,YY2,'Marker','none'); grid on;
hold on; plot(FF(2:2:end),YY2(2:2:end),'o'); hold off;
ax = gca;
% ax.YLim = [0 0.6];

for idx = 2:2000
    yy = m.y((idx-1)*128+1:idx*128,1);
    yy = conj(yy);
    YY = fft(yy(1:Nfft),Nfft);
    YY2 = abs(fftshift(YY));
    ax.Children(2).YData = YY2;
    ax.Children(1).YData = YY2(2:2:end);
    drawnow;
end