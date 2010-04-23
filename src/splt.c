/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "splt.h"

/****************************/
/* splt normal split */

//the real split of the file
static long splt_s_real_split(splt_state *state, int *error, int save_end_point)
{
  double splt_beg = splt_t_get_i_begin_point(state);
  double splt_end = splt_t_get_i_end_point(state);
  char *final_fname = splt_u_get_fname_with_path_and_extension(state,error);
  long new_end_point = splt_u_time_to_long_ceil(splt_end);

  if (*error >= 0)
  {
    double new_sec_end_point = 
      splt_p_split(state, final_fname, splt_beg, splt_end, error, save_end_point);
    new_end_point = splt_u_time_to_long_ceil(new_sec_end_point);

    //if no error
    if (*error >= 0)
    {
      //automatically set progress callback to 100%
      splt_t_update_progress(state,1.0,1.0,1,1,1);

      //we put the split file
      int err = SPLT_OK;
      err = splt_t_put_split_file(state, final_fname);
      if (err < 0) { *error = err; }
    }
  }

  if (final_fname)
  {
    free(final_fname);
    final_fname = NULL;
  }

  return new_end_point;
}

static long splt_s_split(splt_state *state, int first_splitpoint,
    int second_splitpoint, int *error)
{
  int get_error = SPLT_OK;
  long split_begin = splt_t_get_splitpoint_value(state, first_splitpoint, &get_error);
  long split_end = splt_t_get_splitpoint_value(state, second_splitpoint, &get_error);

  long new_end_point = split_end;

  int save_end_point = SPLT_TRUE;
  if (splt_t_get_splitpoint_type(state, second_splitpoint, &get_error) == SPLT_SKIPPOINT ||
      splt_t_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
  {
    save_end_point = SPLT_FALSE;
  }

  if (get_error == SPLT_OK)
  {
    //if no error
    if (*error >= 0)
    {
      //if the first splitpoint different than the end point
      if (split_begin != split_end)
      {
        //convert to float for hundredth
        // 34.6  --> 34 seconds and 6 hundredth
        double splt_beg = split_begin / 100;
        splt_beg += ((split_begin % 100) / 100.);
        double splt_end = 0;

        //LONG_MAX == EOF
        if (split_end == LONG_MAX)
        {
          //Warning : we might not always have total time with input not seekable
          splt_end = splt_t_get_total_time_as_double_secs(state);
        }
        else
        {
          splt_end = split_end / 100;
          splt_end += ((split_end % 100) / 100.);
        }

        splt_t_set_i_begin_point(state, splt_beg);
        splt_t_set_i_end_point(state, splt_end);

        new_end_point = splt_s_real_split(state, error, save_end_point);
      }
      else
      {
        splt_t_set_error_data_from_splitpoint(state, split_begin);
        *error = SPLT_ERROR_EQUAL_SPLITPOINTS;
      }
    }
  }
  else
  {
    *error = get_error;
  }

  return new_end_point;
}

//splits the file with multiple points
void splt_s_multiple_split(splt_state *state, int *error)
{
  int split_type = splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  int err = SPLT_OK;

  splt_t_set_oformat_digits(state);

  if (split_type == SPLT_OPTION_NORMAL_MODE)
  {
    splt_t_put_info_message_to_client(state, _(" info: starting normal split\n"));
  }

  splt_u_print_overlap_time(state);

  int get_error = SPLT_OK;

  splt_array *new_end_points = splt_array_new();

  int i = 0;
  int number_of_splitpoints = splt_t_get_splitnumber(state);
  while (i  < number_of_splitpoints - 1)
  {
    splt_t_set_current_split(state, i);

    if (!splt_t_split_is_canceled(state))
    {
      get_error = SPLT_OK;

      int first_splitpoint_type = splt_t_get_splitpoint_type(state, i, &get_error);
      if (first_splitpoint_type == SPLT_SKIPPOINT)
      {
        splt_u_print_debug(state, "SKIP splitpoint", i, NULL);
        i++;
        continue;
      }

      splt_tu_auto_increment_tracknumber(state);

      long saved_end_point = splt_t_get_splitpoint_value(state, i+1, &get_error);
      splt_u_overlap_time(state, i+1);

      err = splt_u_finish_tags_and_put_output_format_filename(state, i);
      if (err < 0) { *error = err; goto end; }

      long new_end_point = splt_s_split(state, i, i+1, error);
      splt_array_append(new_end_points, (void *)new_end_point);

      splt_t_set_splitpoint_value(state, i+1, saved_end_point);

      if ((*error < 0) || (*error == SPLT_OK_SPLIT_EOF))
      {
        break;
      }
    }
    else
    {
      *error = SPLT_SPLIT_CANCELLED;
      goto end;
    }

    i++;
  }

end:
  for (i = 0;i < splt_array_length(new_end_points);i++)
  {
    splt_t_set_splitpoint_value(state, i+1,
        (long) splt_array_get(new_end_points, i));
  }

  splt_array_free(&new_end_points);
}

void splt_s_normal_split(splt_state *state, int *error)
{
  int output_filenames = splt_t_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);
  if (output_filenames == SPLT_OUTPUT_DEFAULT)
  {
    splt_t_set_oformat(state, SPLT_DEFAULT_OUTPUT, error, SPLT_TRUE);
    if (*error < 0) { return; }
  }

  splt_s_multiple_split(state, error);
}

//the sync error mode
void splt_s_error_split(splt_state *state, int *error)
{
  splt_t_put_info_message_to_client(state, _(" info: starting error mode split\n"));
  char *final_fname = NULL;

  //we detect sync errors
  splt_p_search_syncerrors(state, error);

  //automatically set progress callback to 100% after
  //the error detection
  splt_t_update_progress(state,1.0,1.0,1,1,1);

  int err = SPLT_OK;

  //if no error
  if (*error >= 0)
  {
    //we put the number of sync errors
    splt_t_set_splitnumber(state, state->serrors->serrors_points_num - 1);

    splt_t_set_oformat_digits(state);

    if (splt_t_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES) == SPLT_OUTPUT_DEFAULT)
    {
      splt_t_set_oformat(state, SPLT_DEFAULT_SYNCERROR_OUTPUT, &err, SPLT_TRUE);
      if (err < 0) { *error = err; goto bloc_end; }
    }

    //we split all sync errors
    int i = 0;
    for (i = 0; i < state->serrors->serrors_points_num - 1; i++)
    {
      //if we don't cancel the split
      if (!splt_t_split_is_canceled(state))
      {
        //we put the current file to split
        splt_t_set_current_split(state, i);

        splt_tu_auto_increment_tracknumber(state);

        //we append a splitpoint
        int err = splt_t_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
        if (err < 0) { *error = err; goto bloc_end; }

        err = splt_u_finish_tags_and_put_output_format_filename(state, -1);
        if (err < 0) { *error = err; goto bloc_end; }

        //we get the final fname
        final_fname = splt_u_get_fname_with_path_and_extension(state, error);

        if (*error >= 0)
        {
          splt_u_create_output_dirs_if_necessary(state, final_fname, error);
          if (error < 0)
          {
            goto bloc_end;
          }

          //we split with the detected splitpoints
          int split_result = splt_p_simple_split(state, final_fname, 
              (off_t) state->serrors->serrors_points[i], 
              (off_t) state->serrors->serrors_points[i+1]);

          //automatically set progress callback to 100%
          splt_t_update_progress(state,1.0,1.0,1,1,1);

          if (split_result >= 0)
          {
            *error = SPLT_SYNC_OK;
          }
          else
          {
            *error = split_result;
          }

          //if the split has been a success
          if (*error == SPLT_SYNC_OK)
          {
            err = splt_t_put_split_file(state, final_fname);
            if (err < 0) { *error = err; goto bloc_end; }
          }
        }
        else
        {
          //error
          goto bloc_end;
        }

        //free some memory
        if (final_fname)
        {
          free(final_fname);
          final_fname = NULL;
        }
      }
      //if we cancel the split
      else
      {
        *error = SPLT_SPLIT_CANCELLED;
        goto bloc_end;
      }
    }
  }
  else
  {
    if (*error >= 0 && err < 0)
    {
      *error = err;
    }
  }

bloc_end:
  //free possible unfreed 'final_fname'
  if (final_fname)
  {
    free(final_fname);
    final_fname = NULL;
  }
}

/************************************/
/* splt time and length split */

static void splt_s_split_by_time(splt_state *state, int *error,
    double split_time_length, int number_of_files)
{
  char *final_fname = NULL;
  int j=0, tracks=1;
  double begin = 0.f;
  double end = split_time_length;
  long total_time = splt_t_get_total_time(state);

  if (split_time_length >= 0)
  {
    splt_u_print_overlap_time(state);

    int err = SPLT_OK;

    int temp_int = number_of_files + 1;
    if (number_of_files == -1)
    {
      temp_int = (int)floor(((total_time / 100.0) / (split_time_length))+1) + 1;
    }
    splt_t_set_splitnumber(state, temp_int);

    splt_t_set_oformat_digits(state);

    //if we have the default output
    int output_filenames = splt_t_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES);
    if (output_filenames == SPLT_OUTPUT_DEFAULT)
    {
      splt_t_set_oformat(state, SPLT_DEFAULT_OUTPUT, &err, SPLT_TRUE);
      if (err < 0) { *error = err; return; }
    }

    //we append a splitpoint
    err = splt_t_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
    if (err >= 0)
    { 
      int save_end_point = SPLT_TRUE;
      if (splt_t_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
      {
        save_end_point = SPLT_FALSE;
      }

      int last_file = SPLT_FALSE;

      splt_array *new_end_points = splt_array_new();

      do {
        if (!splt_t_split_is_canceled(state))
        {
          err = splt_t_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
          if (err < 0) { *error = err; break; }

          splt_t_set_current_split(state, tracks-1);

          splt_tu_auto_increment_tracknumber(state);

          int current_split = splt_t_get_current_split(state);

          splt_t_set_splitpoint_value(state, current_split,
              splt_u_time_to_long_ceil(begin));
          long end_splitpoint = splt_u_time_to_long_ceil(end);
          if (total_time > 0 && end_splitpoint >= total_time)
          {
            end_splitpoint = total_time;
            //avoid worst scenarios where floor & SPLT_OK_SPLIT_EOF do not work
            last_file = SPLT_TRUE;
          }
          splt_t_set_splitpoint_value(state, current_split+1, end_splitpoint);

          double overlapped_end = (double)
            ((double)splt_u_overlap_time(state, current_split+1) / 100.0);

          err = splt_u_finish_tags_and_put_output_format_filename(state, -1);
          if (err < 0) { *error = err; break; }

          //we get the final fname
          final_fname = splt_u_get_fname_with_path_and_extension(state,&err);
          if (err < 0) { *error = err; break; }

          double new_sec_end_point = splt_p_split(state, final_fname,
              begin, overlapped_end, error, save_end_point);
          long new_end_point = splt_u_time_to_long_ceil(new_sec_end_point);
          splt_array_append(new_end_points, (void *) new_end_point);

          //if no error for the split, put the split file
          if (*error >= 0)
          {
            err = splt_t_put_split_file(state, final_fname);
            if (err < 0) { *error = err; break; }
          }

          //set new splitpoints
          begin = end;
          end += split_time_length;
          tracks++;

          //get out if error
          if ((*error == SPLT_MIGHT_BE_VBR) ||
              (*error == SPLT_OK_SPLIT_EOF) ||
              (*error < 0))
          {
            tracks = 0;
          }

          if (*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)
          {
            j--;
          }

          if (final_fname)
          {
            //free memory
            free(final_fname);
            final_fname = NULL;
          }

          if (last_file)
          {
            break;
          }
        }
        else
        {
          *error = SPLT_SPLIT_CANCELLED;
          break;
        }

      } while (j++<tracks);

      if (final_fname)
      {
        free(final_fname);
        final_fname = NULL;
      }

      int i = 0;
      for (i = 0;i < splt_array_length(new_end_points);i++)
      {
        splt_t_set_splitpoint_value(state, i+1,
            (long) splt_array_get(new_end_points, i));
      }
      splt_array_free(&new_end_points);
    }
    else
    {
      *error = err;
    }

    //we put the time split error
    switch (*error)
    {
      case SPLT_MIGHT_BE_VBR: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_OK_SPLIT: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_OK_SPLIT_EOF: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_ERROR_BEGIN_OUT_OF_FILE: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      default:
        break;
    }
  }
  else
  {
    *error = SPLT_ERROR_NEGATIVE_TIME_SPLIT;
  }
}

//function used with the -t option (time split
//create an indefinite number of smaller files with a fixed time
//length specified by options.split_time in seconds
void splt_s_time_split(splt_state *state, int *error)
{
  splt_t_put_info_message_to_client(state, _(" info: starting time mode split\n"));

  double split_time_length = (double) splt_t_get_float_option(state, SPLT_OPT_SPLIT_TIME);
  if (((long)split_time_length) == 0)
  {
    *error = SPLT_ERROR_TIME_SPLIT_VALUE_INVALID;
    return;
  }

  splt_s_split_by_time(state, error, split_time_length, -1);
}

//function used with the -L option (length split
//split into X files
//X is defined by SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER
void splt_s_equal_length_split(splt_state *state, int *error)
{
  splt_t_put_info_message_to_client(state, _(" info: starting 'split in equal tracks' mode\n"));

  double total_time = splt_t_get_total_time_as_double_secs(state);
  if (total_time > 0)
  {
    int number_of_files =
      splt_t_get_int_option(state, SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER);

    if (number_of_files > 0)
    {
      double split_time_length = total_time / number_of_files;
      splt_s_split_by_time(state, error, split_time_length, number_of_files);
    }
    else
    {
      *error = SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID;
      return;
    }
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_GET_TOTAL_TIME;
    return;
  }

  if (*error == SPLT_TIME_SPLIT_OK)
  {
    *error = SPLT_LENGTH_SPLIT_OK;
  }
}

/************************************/
/* splt silence detection and split */

//returns the number of silence splits found
//or the number of tracks specified in the options
//sets the silence splitpoints in state->split.splitpoints
int splt_s_set_silence_splitpoints(splt_state *state, int *error)
{
  splt_u_print_debug(state,"We search and set silence splitpoints...",0,NULL);

  //found is the number of silence splits found
  int found = 0;
  int splitpoints_appended = 0;
  struct splt_ssplit *temp = NULL;
  int append_error = SPLT_OK;
  //we get some options
  float offset = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  int number_tracks = splt_t_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);

  //if we have a log file, read silence from logs
  int we_read_silence_from_logs = SPLT_FALSE;
  FILE *log_file = NULL;
  char *log_fname = splt_t_get_silence_log_fname(state);
  if (splt_t_get_int_option(state, SPLT_OPT_ENABLE_SILENCE_LOG))
  {
    if ((log_file = splt_u_fopen(log_fname, "r")))
    {
      char log_silence_fname[1024] = { '\0' };
      fgets(log_silence_fname, 1024, log_file);
      if (log_silence_fname[0] != '\0')
      {
        //remove '\n' at the end
        log_silence_fname[strlen(log_silence_fname)-1] = '\0';
        if (strcmp(log_silence_fname, splt_t_get_filename_to_split(state)) == 0)
        {
          we_read_silence_from_logs = SPLT_TRUE;
          float threshold = SPLT_DEFAULT_PARAM_THRESHOLD;
          float min = SPLT_DEFAULT_PARAM_MINIMUM_LENGTH;
          int i = fscanf(log_file, "%f\t%f", &threshold, &min);

          if ((i < 2) || (threshold != splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD))
              || (splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH) != min))
          {
            we_read_silence_from_logs = SPLT_FALSE;
          }
          else
          {
            splt_t_set_float_option(state, SPLT_OPT_PARAM_THRESHOLD, threshold);
            splt_t_set_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH, min);
          }
        }
      }
      if (!we_read_silence_from_logs && log_file)
      {
        fclose(log_file);
        log_file = NULL;
      }
    }
  }

  //put silence split infos
 
  char remove_str[128] = { '\0' };
  if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
  {
    snprintf(remove_str,128,_("YES"));
  }
  else
  {
    snprintf(remove_str,128,_("NO"));
  }
  char auto_user_str[128] = { '\0' };
  if (splt_t_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS) > 0)
  {
    snprintf(auto_user_str,128,_("User"));
  }
  else
  {
    snprintf(auto_user_str,128,_("Auto"));
  }

  char message[1024] = { '\0' };
  if (! splt_t_get_int_option(state,SPLT_OPT_QUIET_MODE))
  {
    snprintf(message, 1024, _(" Silence split type: %s mode (Th: %.1f dB,"
          " Off: %.2f, Min: %.2f, Remove: %s)\n"),
        auto_user_str,
        splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
        splt_t_get_float_option(state, SPLT_OPT_PARAM_OFFSET),
        splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH),
        remove_str);
    splt_t_put_info_message_to_client(state, message);
  }
 
  if (we_read_silence_from_logs)
  {
    if (state->split.get_silence_level)
    {
      state->split.get_silence_level(0, INT_MAX, state->split.silence_level_client_data);
    }
    snprintf(message, 1024, _(" Found silence log file '%s' ! Reading"
          " silence points from file to save time ;)"), log_fname);
    splt_t_put_info_message_to_client(state, message);
    found = splt_u_parse_ssplit_file(state, log_file, error);
    if (log_file)
    {
      fclose(log_file);
      log_file = NULL;
    }
  }
  else
  {
    if (state->split.get_silence_level)
    {
      //TODO
      state->split.get_silence_level(0, INT_MAX, state->split.silence_level_client_data);
    }
    found = splt_p_scan_silence(state, error);
  }

  //if no error
  if (*error >= 0)
  {
    //put client infos
    char client_infos[512] = { '\0' };
    snprintf(client_infos,512,_("\n Total silence points found: %d."),found);
    splt_t_put_info_message_to_client(state,client_infos);
    if (found > 0)
    {
      int selected_tracks = found + 1;
      int param_number_of_tracks = splt_t_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);
      if (param_number_of_tracks > 0)
      {
        selected_tracks = param_number_of_tracks;
      }
      snprintf(client_infos,512,_(" (Selected %d tracks)\n"), selected_tracks);
      splt_t_put_info_message_to_client(state, client_infos);
    }
    else
    {
      snprintf(client_infos, 512, "\n");
      splt_t_put_info_message_to_client(state,client_infos);
    }

    //we set the number of tracks
    if (!splt_t_split_is_canceled(state))
    {
      found++;
      if ((number_tracks > 0) &&
          (number_tracks < SPLT_MAXSILENCE))
      {
        if (number_tracks < found)
        {
          found = number_tracks;
        }
      }

      //put first splitpoint
      append_error = splt_t_append_splitpoint(state, 0, NULL, SPLT_SPLITPOINT);
      if (append_error != SPLT_OK)
      {
        *error = append_error;
      }
      else
      {
        temp = state->silence_list;
        int i;

        //we take all splitpoints found and we remove silence 
        //if needed
        for (i = 1; i < found; i++)
        {
          if (temp == NULL)
          {
            found = i;
            break;
          }

          if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
          {
            append_error = splt_t_append_splitpoint(state, 0, NULL, SPLT_SKIPPOINT);
            if (append_error < 0) { *error = append_error; found = i; break;}
            append_error = splt_t_append_splitpoint(state, 0, NULL, SPLT_SPLITPOINT);
            if (append_error < 0) { *error = append_error; found = i; break;}
            splt_t_set_splitpoint_value(state, 2*i-1,splt_u_time_to_long(temp->begin_position));
            splt_t_set_splitpoint_value(state, 2*i, splt_u_time_to_long(temp->end_position));
          }
          else
          {
            //TODO
            long temp_silence_pos = splt_u_silence_position(temp, offset) * 100;
            append_error = splt_t_append_splitpoint(state, temp_silence_pos, NULL, SPLT_SPLITPOINT);
            if (append_error != SPLT_OK) { *error = append_error; found = i; break; }
          }
          temp = temp->next;
        }

        //we order the splitpoints
        if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
        {
          splitpoints_appended = (found-1)*2+1;
        }
        else 
        {
          splitpoints_appended = found;
        }

        splt_u_print_debug(state,"We order splitpoints...",0,NULL);
        splt_u_order_splitpoints(state, splitpoints_appended);

        //last splitpoint, end of file
        append_error =
          splt_t_append_splitpoint(state, splt_t_get_total_time(state),
              NULL, SPLT_SPLITPOINT);
        if (append_error != SPLT_OK) { *error = append_error; }
      }
    }
    else
    {
      *error = SPLT_SPLIT_CANCELLED;
    }

    //if splitpoints are found
    if ((found > 0) && !we_read_silence_from_logs)
    {
      //if we write the silence points log file
      if (splt_t_get_int_option(state, SPLT_OPT_ENABLE_SILENCE_LOG))
      {
        char *message = malloc(sizeof(char) * 1024);
        if (message)
        {
          snprintf(message, 1023, _(" Writing silence log file '%s' ...\n"),
              splt_t_get_silence_log_fname(state));
          splt_t_put_info_message_to_client(state, message);
          if (message)
          {
            free(message);
            message = NULL;
          }
          char *fname = splt_t_get_silence_log_fname(state);
          if (! splt_t_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
          {
            FILE *log_file = NULL;
            if (!(log_file = splt_u_fopen(fname, "w")))
            {
              splt_t_set_strerror_msg(state);
              splt_t_set_error_data(state, fname);
              *error = SPLT_ERROR_CANNOT_OPEN_FILE;
            }
            else
            {
              //do the effective write
              struct splt_ssplit *temp = state->silence_list;
              fprintf(log_file, "%s\n", splt_t_get_filename_to_split(state));
              fprintf(log_file, "%.2f\t%.2f\n", 
                  splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
                  splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH));
              while (temp != NULL)
              {
                fprintf(log_file, "%f\t%f\t%ld\n",
                    temp->begin_position, temp->end_position, temp->len);
                temp = temp->next;
              }
              fflush(log_file);
              if (log_file)
              {
                fclose(log_file);
                log_file = NULL;
              }
              temp = NULL;
            }
          }
        }
        else
        {
          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        }
      }
    }
  }
  splt_t_ssplit_free(&state->silence_list);

  splt_t_set_splitnumber(state, splitpoints_appended + 1);

  return found;
}

//do the silence split
//possible error in error
void splt_s_silence_split(splt_state *state, int *error)
{
  splt_u_print_debug(state,"Starting silence split ...",0,NULL);

  //print some useful infos to the client
  splt_t_put_info_message_to_client(state, _(" info: starting silence mode split\n"));

  int found = 0;
  found = splt_s_set_silence_splitpoints(state, error);

  //if no error
  if (*error >= 0)
  {
    //if we have found splitpoints, write the silence tracks
    if (found > 1)
    {
      //we put the number of tracks found
      splt_u_print_debug(state,"Writing silence tracks...",0,NULL);

      //set the default silence output
      int output_filenames = splt_t_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);
      if (output_filenames == SPLT_OUTPUT_DEFAULT)
      {
        splt_t_set_oformat(state, SPLT_DEFAULT_SILENCE_OUTPUT, error, SPLT_TRUE);
        if (*error < 0) { return; }
      }

      splt_s_multiple_split(state, error);

      //we put the silence split errors
      switch (*error)
      {
        case SPLT_MIGHT_BE_VBR:
          *error = SPLT_SILENCE_OK;
          break;
        case SPLT_OK_SPLIT:
          *error = SPLT_SILENCE_OK;
          break;
        case SPLT_OK_SPLIT_EOF:
          *error = SPLT_SILENCE_OK;
          break;
        default:
          break;
      }
    }
    else
    {
      *error = SPLT_NO_SILENCE_SPLITPOINTS_FOUND;
    }
  }
}

/****************************/
/* splt wrap split */

//do the wrap split
void splt_s_wrap_split(splt_state *state, int *error)
{
  char *new_filename_path = splt_t_get_new_filename_path(state);
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug(state,"We begin wrap split for the file ...",0,filename);

  splt_t_put_info_message_to_client(state, _(" info: starting wrap mode split\n"));

  splt_p_dewrap(state, SPLT_FALSE, new_filename_path, error);
}

