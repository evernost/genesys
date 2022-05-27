close all
clear all
clc

%% Settings

T_upd = 0.0024;

p_x = [0.0; 0.0500000000000; 0.1419792059627; 0.2703860378231; 1.0107343407361];
p_y = [0.0; 0.7500000000000; 0.9033872500406; 0.9509988607702; 1.0];

s = zeros(4,1);

s(1) = -(p_y(5)-p_y(4))/(p_x(5)-p_x(4));
s(2) = (p_y(2)-p_y(1))/(p_x(2)-p_x(1));
s(3) = (p_y(3)-p_y(2))/(p_x(3)-p_x(2));
s(4) = (p_y(4)-p_y(3))/(p_x(4)-p_x(3));

a = [-1,-1,-1,-1; 1,-1,-1,-1; 1,1,-1,-1; 1,1,1,-1]\s;

x = linspace(0.0, 1.0, 100)';
y = a(1)*abs(x) + a(2)*(abs(x-p_x(2))-p_x(2)) + a(3)*(abs(x-p_x(3))-p_x(3)) + a(4)*(abs(x-p_x(4))-p_x(4));

t = -3*T_upd./log(y);

subplot(2,1,1)
plot(x,y,p_x,p_y,'o')
grid on

subplot(2,1,2)
plot(x,t)
xlabel('normalized time input')
ylabel('t_{95} time (s)')
grid on

C = a(2)*(-p_x(2)) + a(3)*(-p_x(3)) + a(4)*(-p_x(4));
t_95_max = t(100);

fprintf('Update period = %2.15f\n', T_upd);
fprintf('a0            = %2.15f\n', a(1));
fprintf('a1            = %2.15f\n', a(2));
fprintf('a2            = %2.15f\n', a(3));
fprintf('a4            = %2.15f\n', a(4));
fprintf('\n');
fprintf('p0            = %2.15f\n', 0);
fprintf('p1            = %2.15f\n', p_x(2));
fprintf('p2            = %2.15f\n', p_x(3));
fprintf('p3            = %2.15f\n', p_x(4));
fprintf('\n');
fprintf('C             = %2.15f\n', C);
fprintf('\n');
fprintf('t95 max.      = %2.15f\n', t_95_max);


