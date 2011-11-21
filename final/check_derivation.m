function check_derivation

%@T
% \section{Sanity checks}
%
% I fairly regularly make typographical and copying errors when I do
% algebra and implement it in code.  In order to stay sane when I
% actually write something somewhat complicated, I find it helpful to
% put together little test scripts to check my work numerically.
% For your edification, in this section I give my MATLAB test script 
% corresponding to the derivation in these notes.  The test script 
% is done in MATLAB.
%
% I begin by implementing the functions $f(q)$, the normalizing constants,
% and the kernel functions for each of the three kernels.
%@c
fp6 = @(q) (1-q^2)^3;
fsp = @(q) (1-q)^3;
fvi = @(q) q^2/4 - q^3/9 - log(q)/6 - 5/36;
Cp6 = pi/4;
Csp = pi/10;
Cvi = pi/40;
Wp6 = @(r,h) 1/Cp6/h/h * fp6( norm(r)/h );
Wsp = @(r,h) 1/Csp/h/h * fsp( norm(r)/h );
Wvi = @(r,h) 1/Cvi/h/h * fvi( norm(r)/h );

%@T
%
% I computed the normalization constants analytically, but I'm
% prone to algebra mistakes when I compute integrals by hand.
% Let's check against MATLAB's [[quad]] function.
%@c
fprintf('Relerr for normalization constants:\n');
nerr_p6 = quad( @(q) 2*pi*q*fp6(q)/Cp6, 0,     1 ) - 1;
nerr_sp = quad( @(q) 2*pi*q*fsp(q)/Csp, 0,     1 ) - 1;
nerr_vi = quad( @(q) 2*pi*q*fvi(q)/Cvi, 1e-12, 1 ) - 1;
fprintf('  Cp6: %g\n', nerr_p6);
fprintf('  Csp: %g\n', nerr_sp);
fprintf('  Cvi: %g\n', nerr_vi);

%@T
%
% Now check that I did the calculus right for the gradient and
% Laplacian of the $\Wps$ kernel and for the gradient of the
% pressure kernel.
%@c
h = rand(1);
r = rand(2,1)*h/4;
q = norm(r)/h;
r2 = r'*r;
h2 = h^2;
dr = norm(r)*1e-4;
gWp6_fd = fd_grad(@(r) Wp6(r,h), r, dr);
gWp6_ex = -24/pi/h^8*(h2-r2)^2*r;
gWsp_fd = fd_grad(@(r) Wsp(r,h),r,dr);
gWsp_ex = -(30/pi)/h^4 * (1-q)^2 / q * r;
lWp6_fd = fd_laplace(@(r) Wp6(r,h), r, dr);
lWp6_ex = -48/pi/h^8*(h2-r2)*(h2-3*r2);
fprintf('Check Wp6 kernel derivatives:\n');
fprintf('  grad Wp6:  %g\n', norm(gWp6_fd-gWp6_ex)/norm(gWp6_ex));
fprintf('  grad Wsp:  %g\n', norm(gWsp_fd-gWsp_ex)/norm(gWsp_ex));
fprintf('  laplacian: %g\n', (lWp6_fd-lWp6_ex)/lWp6_ex);

%@T
%
% Now check that $\fWvi(q)$ satisfies the conditions that
% are supposed to define it:
% \begin{align*}
%   f(1) &= 0 \\
%   f'(1) &= 0 \\
%   f''(q) + \frac{1}{q} f'(q) &= 1-q, \quad 0 < q < 1
% \end{align*}
% The first two conditions we check directly; the last we check
% at a randomly chosen $q$.
%@c
q    = rand(1);
dq   = 1e-4*q;
fq   = fvi(q);
lffd = fd_laplace_radial(fvi,q,dq);
fprintf('Relerr for viscosity kernel checks:\n');
fprintf('  fvi (1): %g\n', fvi(1) );
fprintf('  dfvi(1): %g\n', fd_deriv(fvi,1,dq) );
fprintf('  Laplace: %g\n', fd_laplace_radial(fvi,q,dq)/(1-q)-1);


%@T
%
% Now, let me check that I did the algebra right in getting the
% condensed formula for the interaction forces.
%@c

% Set up random parameter choices
r_ij = rand(2,1);
v_ij = rand(2,1);
k    = rand(1);
rho0 = rand(1);
rhoi = rand(1);
rhoj = rand(1);
mass = rand(1);
mu   = rand(1);
q    = norm(r_ij)/h;

% Compute pressures via equation of state
Pi = k*(rhoi-rho0);
Pj = k*(rhoj-rho0);

% Differentiate the kernels 
Wsp_x  = -30/pi/h^4*(1-q)^2/q*r_ij;
LWvi   = 40/pi/h^4*(1-q);

% Do the straightforward computation
fpressure  = -mass*(Pi+Pj)/2/rhoj * Wsp_x;
fviscous   = -mu*mass*v_ij/rhoj * LWvi;
finteract1 = fpressure + fviscous;

% Do the computation based on my condensed formula
finteract2 = mass/pi/h^4/rhoj * (1-q) * ...
    ( 15*k*(rhoi+rhoj-2*rho0)*(1-q)/q * r_ij - ...
      40*mu * v_ij );

% Compare
fprintf('Relerr in interaction force check:\n');
fprintf('  fint:  %g\n', norm(finteract1-finteract2)/norm(finteract1));

%@T
%
% Of course, all the above is supported by a number of little
% second-order accurate finite difference calculations.
%@c

function fp   = fd_deriv(f,r,h)
  fp = ( f(r+h)-f(r-h) )/2/h;

function fpp = fd_deriv2(f,r,h)
  fpp = ( f(r+h)-2*f(r)+f(r-h) )/h/h;

function del2f = fd_laplace_radial(f,r,h)
  del2f = fd_deriv2(f,r,h) + fd_deriv(f,r,h)/r;

function del2f = fd_laplace(f,r,h)
  e1 = [1; 0];
  e2 = [0; 1];
  del2f = (-4*f(r)+f(r+h*e1)+f(r+h*e2)+f(r-h*e1)+f(r-h*e2) )/h/h;

function gradf = fd_grad(f,r,h)
  e1 = [1; 0];
  e2 = [0; 1];
  gradf = [f(r+h*e1)-f(r-h*e1); 
           f(r+h*e2)-f(r-h*e2)] / 2 / h;

  
