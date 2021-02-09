static const int N = 10;

typedef struct
{
  float ts[N];
  void (*fns[N])();
  int schedule_i; // where to schedule iterator
  int execute_i; // what to execute iterator
  int count; // count
} schedule_data_t;


void reset_schedule(schedule_data_t* schedule_data)
{
  schedule_data->schedule_i = 0;
  schedule_data->execute_i = 0;
  schedule_data->count = 0;
}


void add_schedule(schedule_data_t* schedule_data, const float t, void (*fn)())
{
  schedule_data->ts[schedule_data->schedule_i] = t;
  schedule_data->fns[schedule_data->schedule_i] = fn;

  schedule_data->schedule_i = (schedule_data->schedule_i + 1) % N;
  schedule_data->count += 1;
}


void execute_schedule(schedule_data_t* schedule_data, const float t)
{
  while (schedule_data->count > 0 &&
      schedule_data->ts[schedule_data->execute_i] < t) {
    schedule_data->fns[schedule_data->execute_i]();

    schedule_data->count -= 1;
    schedule_data->execute_i = (schedule_data->execute_i + 1) % N;
  }
}
