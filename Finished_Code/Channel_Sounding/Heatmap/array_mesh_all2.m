% Created by Dakota Flanary
% Adds the data from the second location to the whole array

Nfft = 2*64;
tx2xx = zeros(90,345,Nfft);

% num_rows = 26;
% num_runs = 32;
% Nfft = 2*64;
% FF = -0.5:1/Nfft:0.5-1/Nfft;
% FF = 20*FF;
% tx2camacho_temp = zeros(num_rows,num_runs,Nfft);
% tx2camacho = zeros(num_runs,num_rows,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "tx2camacho_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         tx2camacho_temp(row, runs,:) = YYplot;
%     end
% end
% for run = 1:num_runs
%     for row = 1:num_rows
%         tx2camacho(run,row,:) = tx2camacho_temp(row,run,:);
%     end
% end
% num_rows = 30;
% num_runs = 34;
% tx2harrison = zeros(num_rows,num_runs,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "tx2harrison_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         tx2harrison(row, runs,:) = YYplot;
%     end
% end
% num_rows = 32;
% num_runs = 26;
% tx2smalley = zeros(num_rows,num_runs,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "tx2smalley_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         tx2smalley(row, runs,:) = YYplot;
%     end
% end
% 
% num_rows = 28;
% num_runs = 32;
% tx2chambers = zeros(num_runs, num_rows, Nfft);
% for row = 1:num_rows
%     s = "tx2chambers_" + string(row);
%     hh = eval(s);
%     for run = 1:num_runs
%         if row < 27
%             YY = pwelch(hh(:,run),boxcar(Nfft),0,Nfft,'twosided');
%             YYplot = 10*log10(abs(fftshift(YY)));
%             tx2chambers(run,row,:) = YYplot;
%         end
%         if row > 26
%             if run < 30
%                 YY = pwelch(hh(:,run),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 tx2chambers(run,row,:) = YYplot;
%             end
%         end
%     end
% end
% 
% num_rows = 65;
% num_runs = 61;
% tx2conference = zeros(num_rows,num_runs,Nfft);
% for row = 1:num_rows
%     s = "tx2conference_" + string(row);
%     hh = eval(s);
%     for runs = 1:num_runs
%         if row < 5
%             if runs < 31
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 tx2conference(row, runs+31,:) = YYplot;
%             end
%         elseif row < 8
%             if runs < 59
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 tx2conference(row, runs+3,:) = YYplot;
%             end
%         elseif row < 63
%             YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%             YYplot = 10*log10(abs(fftshift(YY)));
%             tx2conference(row, runs,:) = YYplot;
%         else
%             if runs < 56
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 tx2conference(row, runs+3,:) = YYplot;
%             end
%         end 
%     end
% end

% Adds the data from the conference room to the whole array
num_rows = 65;
num_runs = 61;
for row = 1:num_rows
    for run = 1:num_runs
        tx2xx(row,run,:) = tx2conference(row,run,:);
    end
end
% Adds the data from hallway to the whole array
% num_rows = 23;
% num_runs = 300;
% for row = 1:num_rows
%     for run = 1:num_runs
%         tx2xx(row+67,run+40,:) = tx2hallway(row,run,:);
%     end
% end
% Adds the data from the from Dr. Harrison's office to the whole array
num_rows = 30;
num_runs = 34;
for row = 1:num_rows
    for run = 1:num_runs
        tx2xx(35+row,63+run,:) = tx2harrison(row,run,:);
    end
end
% Adds the data from Dr. Smalley's office to the whole array
num_rows = 32;
num_runs = 26;
for row = 1:num_rows
    for run = 1:num_runs
        tx2xx(33+row,119+run,:) = tx2smalley(row,run,:);
    end
end
% Adds the data from Dr. Camacho's office to the whole array
num_rows = 32;
num_runs = 26;
for row = 1:num_rows
    for run = 1:num_runs
        tx2xx(33+row,149+run,:) = tx2camacho(row,run,:);
    end
end
% Adds the data from Michael Chambers office to the whole array
num_rows = 32;
num_runs = 28;
for row = 1:num_rows
    for run = 1:num_runs
        tx2xx(33+row, 207+run,:) = tx2chambers(row,run,:);
    end
end