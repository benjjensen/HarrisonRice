Fs = 100;
F0 = 0.001;
N = 1000;
myalpha = 0.1;
stride = 1000 * N;

filename = '/media/V2V/PROVO-LOW_follow-straight_2019-01-29__15-49-37.mat';
m = matfile(filename);

temp = m.data(1:N,1);
x = (temp - 8192)/8192;
X = abs(fft(x,N));

figure(11);
plot(FF,20*log10(fftshift(X)));
grid on;
ax = gca;

ax.XLim = 5*[-1 1];
ax.YLim = [-60 60];
ax.XLabel.String = 'frequency';
ax.YLabel.String = 'magnitude (dB)';
% ax.Title.String = sprintf('index = %4d',0);
ax.Children.Color = 'k';
ax.Children.LineWidth = 1;

n1 = N;
for idx = 1:10720
    midx = (n1+1):(n1+N);
    temp = m.data(midx,1);
    x = (temp - 8192)/8192;
 
    X = myalpha*X + (1-myalpha)*abs(fft(x,N));
    ax.Children.YData = 20*log10(fftshift(X));
    % ax.Title.String = sprintf('index = %4d',idx);
    drawnow;
    
    n1 = n1 + stride;
end