DEPTHBOUND = 5;

double ABsearch(int n, double A, double B)
{
  if (n >= DEPTHBOUND)
  {
    return eval(n);
  }
  else
  {
    $succ = array();
    $succ = getsuc(n);
    if (ismax(n))
    {
      for(k = 0; k < length($succ); ++k)
      {
        a = max(A, ABsearch($succ[k],A,B))
        if (a >= B) return B;
      }
    }
    else (ismin(n))
    {
      
    }
  }


}