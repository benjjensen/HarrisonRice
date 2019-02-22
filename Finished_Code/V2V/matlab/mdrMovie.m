%%% mdrMovie

Fs = 100;
F0 = 0.3;
N = 5*64; % period at 100 Msamples/s
L = 50;

% Fs = 100;
% F0 = 0.001;
% N = 1000;
% L = 50;

stride = 1000*N; % numsamples needs to be multiple of 100 so the indexing 
               % of the complex exponential all works
myalpha = 0.95;

h = firls(2*L,[0 0.1 0.15 0.5]*2,[1 1 0 0]);

% Nfft = 1024;
% H = freqz(h,1,Nfft,'whole');
% FF = -0.5:1/Nfft:0.5-1/Nfft;
% plot(FF,20*log10(abs(fftshift(H)))); grid on;

FF = (-0.5:1/N:0.5-1/N)'*Fs;

% Live test, no frequency correction:
% filename = '/media/V2V/indoor-test_2019-02-06__14-46-25.mat';

% No frequency correction:
% filename = '/media/V2V/indoor-test_2019-02-13__14-39-42.mat';

% Some frequency correction:
% filename = '/media/V2V/indoor-test_2019-02-13__15-15-50.mat';

% Good frequency correction:
% filename = '/media/V2V/indoor-test_2019-02-13__15-21-10.mat';

% Live test with Tx and Rx, with good (but not perfect) frequency
% correction:
% filename = '/media/V2V/indoor-test_2019-02-13__15-47-10.mat';

% Old signal, near perfect frequency correction:
% filename = '/media/V2V/indoor-test_2019-02-14__13-14-54.mat';

% New signal:
% filename = '/media/V2V/indoor-test_2019-02-14__15-27-55.mat';

% Lindsey's data:
% filename = '/media/V2V/lindsey_hallway_data/stationary_2019-02-14__17-43-30.mat';
%filename = '/media/V2V/lindsey_hallway_dat/moving_away_2019-02-14__17-44-15.mat';
% filename = '/media/V2V/lindsey_hallway_data/moving_towards_2019-02-14__17-43-52.mat';

filename = '/media/V2V/lindsey_hallway_data/incorrect-data/stationary_2019-02-14__16-42-10.mat';

m = matfile(filename);

%%% get the first N to create the plot, define the axes, 
%%% and set the plot parameters

temp = m.data(1:N,1);
x = (temp - 8192)/8192;
y1 = x.*exp(-1i*2*pi*F0*(0:N-1)');
y2 = conv(y1,h);
Y2 = abs(fft(y2(L+1:end-L),N));

figure(10);
plot(FF,20*log10(fftshift(Y2)));
grid on;
ax = gca;

figure(11);
plot(x);
grid on;
ax2 = gca;
% ax2.YLim = .3 * [-1 1];
ax2.YLim = 1 * [-1 1];

% figure(12);
% plot(abs(y2));
% grid on;
% ax3 = gca;
% ax3.YLim = [0, 1];

ax.XLim = 20*[-1 1];
ax.YLim = [-60 60];
ax.XLabel.String = 'frequency';
ax.YLabel.String = 'magnitude (dB)';
% ax.Title.String = sprintf('index = %4d',0);
ax.Children.Color = 'k';
ax.Children.LineWidth = 1;


% loop through the data numSamples at a time and update the plot

n1 = N;
for idx = 1:10720
    
    % pause(0.5);
    
    midx = (n1+1):(n1+N);
    temp = m.data(midx,1);
    x = (temp - 8192)/8192;
    ax2.Children.YData = x;
    
    y1 = x.*exp(-1i*2*pi*F0*(0:N-1)');
    % ax3.Children.YData = abs(y1);
    % ax2.Title.String = sprintf('starting sample = %d',n1);
    
    y2 = conv(y1,h);
 
    Y2 = myalpha*Y2 + (1-myalpha)*abs(fft(y2(L+1:end-L),N));
    % Y2 = max(Y2,abs(fft(y2(L+1:end-L),N)));
    ax.Children.YData = 20*log10(fftshift(Y2));
    % ax.Title.String = sprintf('starting sample = %d',n1);
    
    drawnow;
    
    n1 = n1 + stride;
end
