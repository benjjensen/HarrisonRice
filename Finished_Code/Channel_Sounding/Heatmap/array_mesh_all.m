% Created by Dakota Flanary
% Combines all of the data from the separate rooms into one matrix.

Nfft = 2*64;
xx = zeros(90,345,Nfft);
% 
% num_rows = 26;
% num_runs = 32;
% Nfft = 2*64;
% FF = -0.5:1/Nfft:0.5-1/Nfft;
% FF = 20*FF;
% camacho_temp = zeros(num_rows,num_runs,Nfft);
% camacho = zeros(num_runs,num_rows,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "camacho_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         camacho_temp(row, runs,:) = YYplot;
%     end
% end
% for run = 1:num_runs
%     for row = 1:num_rows
%         camacho(run,row,:) = camacho_temp(row,run,:);
%     end
% end
% 
% num_rows = 29;
% num_runs = 33;
% harrison = zeros(num_rows,num_runs,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "harrison_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         harrison(row, runs,:) = YYplot;
%     end
% end
% 
% num_rows = 31;
% num_runs = 26;
% smalley = zeros(num_rows,num_runs,Nfft);
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "smalley_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         smalley(row, runs,:) = YYplot;
%     end
% end
% 
% 
% num_rows = 28;
% num_runs = 32;
% chambers = zeros(num_runs, num_rows, Nfft);
% for row = 1:num_rows
%     s = "chambers_" + string(row);
%     hh = eval(s);
%     for run = 1:num_runs
%         if row < 27
%             YY = pwelch(hh(:,run),boxcar(Nfft),0,Nfft,'twosided');
%             YYplot = 10*log10(abs(fftshift(YY)));
%             chambers(run,row,:) = YYplot;
%         end
%         if row > 26
%             if run < 30
%                 YY = pwelch(hh(:,run),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 chambers(run,row,:) = YYplot;
%             end
%         end
%     end
% end
% 
% conference = [];
% num_rows = 65;
% num_runs = 61;
% for row = 1:num_rows
%     for runs = 1:num_runs
%         if row < 4
%             if runs < 56
%                 s = "conference_north_" + string(row);
%                 hh = eval(s);
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 conference(row, runs+3,:) = YYplot;
%             end
%         elseif row < 59
%             s = "conference_main_" + string(row-3);
%             hh = eval(s);
%             YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%             YYplot = 10*log10(abs(fftshift(YY)));
%             conference(row, runs,:) = YYplot;
%         elseif row < 62
%             if runs < 59
%                 s = "conference_south_pillar_" + string(row-58);
%                 hh = eval(s);
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 conference(row, runs,:) = YYplot;
%             end
%         else
%             if runs < 30
%                 s = "conference_south_wall_" + string(row-61);
%                 hh = eval(s);
%                 YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%                 YYplot = 10*log10(abs(fftshift(YY)));
%                 conference(row, runs,:) = YYplot;
%             end
%         end 
%     end
% end

% Adds the conference room data to the whole array
num_rows = 65;
num_runs = 61;
for row = 1:num_rows
    for run = 1:num_runs
        xx(row,run,:) = conference(num_rows+1-row,num_runs+1-run,:);
    end
end
% Adds the hallway data to the whole array
num_rows = 23;
num_runs = 300;
for row = 1:num_rows
    for run = 1:num_runs
        xx(row+67,run+40,:) = hall_pwelch(run,num_rows+1-row,:);
    end
end
% Adds the data from Dr. Harrison's room to the array
num_rows = 29;
num_runs = 33;
for row = 1:num_rows
    for run = 1:num_runs
        xx(36+row,63+run,:) = harrison(num_rows+1-row,run,:);
    end
    
end
% Adds the data from Dr. Smalley's office to the whole array
num_rows = 31;
num_runs = 26;
for row = 1:num_rows
    for run = 1:num_runs
        xx(34+row,119+run,:) = smalley(num_rows+1-row,num_runs+1-run,:);
    end
end
% Adds the data from Dr. Camacho's office to the whole array
num_rows = 32;
num_runs = 26;
for row = 1:num_rows
    for run = 1:num_runs
        xx(33+row,149+run,:) = camacho(num_rows+1-row,run,:);
    end
end
% Adds the data from Michael Chambers office to the whole array
num_rows = 32;
num_runs = 28;
for row = 1:num_rows
    for run = 1:num_runs
        xx(33+row, 207+run,:) = chambers(row,run,:);
    end
end
