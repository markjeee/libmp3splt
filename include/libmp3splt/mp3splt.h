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

#ifndef MP3SPLT_MP3SPLT_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

//libtool 1.4e is buggy on mingw if we include ltdl.h
#ifndef __WIN32__
#include <ltdl.h>
#endif

/**
 * @file  mp3splt.h
 * 
 * @brief Main types, error codes and confirmations.
 * 
 * This is the main file containing the most important types, error
 * codes and confirmations
 *
 * The errors are negative values, the warnings and the confirmations
 * are positive values
 */

/**
 * @brief True value
 */
#define SPLT_TRUE 1
/**
 * @brief False value
 */
#define SPLT_FALSE 0

/******************************/
/* Structures for the freedb  */

/**
 * @brief Defines one search result from the freedb search
 *
 * @see splt_freedb_results
 * @see mp3splt_get_freedb_search
 */
typedef struct {
  /**
   * @brief Name of the album for this result
   */
  char *name;
  /**
   * @brief Unique identifier for this result
   */
  int id;
  /**
   * @brief How many revisions this result has
   *
   * If having revisions, the unique identifier for a revision is
   * #id + revision + 1, with 0 <= revision < #revision_number\n
   */
  int revision_number;
  /**
   * @brief Contains the revisions of the result
   * 
   * The #revisions table contains #revision_number elements,
   * the consecutive numbers starting at 2 (you might not need it)
   */
  int *revisions;
} splt_freedb_one_result;

/**
 * @brief All the freedb search results
 *
 * @see splt_freedb_one_result
 * @see mp3splt_get_freedb_search
 */
typedef struct {
  /**
   * @brief All the freedb results
   */
  splt_freedb_one_result *results;
  /**
   * @brief How many results we have
   */
  int number;
} splt_freedb_results;

/**
 * @brief Maximum results for the freedb search
 */
#define SPLT_MAXCD 512

//maximum length of the disc id
#define SPLT_DISCIDLEN 8

//structure for the freedb search
struct splt_cd {
  char discid[SPLT_DISCIDLEN+1];
  char category[20];
};

typedef struct {
  struct splt_cd discs[SPLT_MAXCD];
  int foundcd;
} splt_cd_state;

//structure containing everything used for the
//freedb search
typedef struct {
  //we stock here the results of the freedb search
  splt_freedb_results *search_results;
  //we stock the state of the CD
  //(for the freedb search)
  splt_cd_state *cdstate;
} splt_freedb;

/******************************/
/* Structures for the wrap    */

/**
 * @brief The wrapped filenames found inside a file
 *
 * @see mp3splt_get_wrap_files
 */
typedef struct {
  /**
   * @brief How many filenames we have
   */
  int wrap_files_num;
  /**
   * @brief The filenames
   */
  char **wrap_files;
} splt_wrap;

/************************************/
/* Structures for the syncerrors    */

/**
 * @brief The number of syncerrors
 *
 * @see mp3splt_get_syncerrors
 */
typedef struct {
  off_t *serrors_points;
  /**
   * @brief How many syncerrors have been found
   */
  long int serrors_points_num;
} splt_syncerrors;

/***************************************/
/* Structures for the output format    */

#define SPLT_MAXOLEN 255
#define SPLT_OUTNUM  20

//structure defining the output format
typedef struct {
  //format as @n_@t.. as a string
  char *format_string;
  //when we have @n option on output format
  char output_format_digits;
  int output_alpha_format_digits;
  //format for the cddb cue output
  char format[SPLT_OUTNUM+1][SPLT_MAXOLEN];
} splt_oformat;

/***************************/
/* Structures for the tags */

/**
 * @brief The tags of a splitpoint
 *
 * The structure contains the tags that we can set to a filename
 * generated from a splitpoint. Tags may also define the output filenames.
 *
 * @see mp3splt_append_tags
 * @see mp3splt_get_tags
 */
typedef struct {
  /**
   * @brief The title
   */
  char *title;
  /**
   * @brief The artist
   */
  char *artist;
  /**
   * @brief The album
   */
  char *album;
  /**
   * @brief The performer
   * 
   * The performer is not part of the tags, but may replace the
   * #artist in some cases and it is useful for the output filenames\n
   * You can also look at #SPLT_OPT_OUTPUT_DEFAULT
   */
  char *performer;
  /**
   * @brief The year
   */
  char *year;
  /**
   * @brief A comment
   */
  char *comment;
  /**
   * @brief The track number
   */
  int track;
  /**
   * @brief The genre
   */
  unsigned char genre;

  /*
   * @brief tags version (for mp3): 1 or 2 or 1 & 2
   */
  int tags_version;
} splt_tags;

/**
 * @brief Definition of a splitpoint
 *
 * @see mp3splt_append_splitpoint
 * @see mp3splt_get_splitpoints
 */
typedef struct {
  /**
   * @brief Value of the splitpoint in hundreths of seconds
   */
  long value;
  /**
   * @brief Name of the new filename issued from the splitpoint
   */
  char *name;
  /**
   * @brief Type of the splitpoint
   * @see splt_type_of_splitpoint
   */
  int type;
} splt_point;

/*****************************/
/* Structure for the silence */

struct splt_ssplit {
  double begin_position;
  double end_position;
  long len;
  struct splt_ssplit *next;
};

/**********************************/
/* Structure for the split        */

/**
 * @brief must be documented
 */
typedef struct splt_progres {
  //maximum number of characters for the filename(without the
  //extension) when displaying the progress
  //warning, don't set this more than 512 !
  //default is 40
  int progress_text_max_char;
  //filename that we are currently splitting
  char filename_shorted[512];
  //the current percent of the progress
  float percent_progress;
  //the splitpoint that we are currently splitting
  int current_split;
  //the maximum number of splits
  int max_splits;
  //the progress type
  //can be:
  //SPLT_PROGRESS_PREPARE
  //SPLT_PROGRESS_CREATE
  //SPLT_PROGRESS_SEARCH_SYNC
  //SPLT_PROGRESS_SCAN_SILENCE
  int progress_type;
  //infos for the silence split
  int silence_found_tracks;
  float silence_db_level;
  //use this variable as you wish
  //this variable will not be modified by the library
  //but is 0 at the start
  int user_data;
  //float = fraction of how much %
  //char * = name on the progress bar
  void (*progress)(struct splt_progres*);
} splt_progress;

//proxy informations
typedef struct {
  //if we use proxy or not
  int use_proxy;
  char hostname[256];
  int port;
  //if we need authentification or not
  int authentification;
  //the user and password needed for authentification
  char user[256];
  char password[256];
} splt_proxy;

//used with the 'put_message' function
typedef enum {
  /**
   * Info message
   */
  SPLT_MESSAGE_INFO,
  /**
   * Debug message
   */
  SPLT_MESSAGE_DEBUG
} splt_message_type;

typedef struct {
  //total time of the song
  long total_time;
  //the part of file that we are currently splitting
  //1 if the first, 2 if the second
  //the number of splitpoints
  int current_split;
  //for the user feedback: the current split file number
  int current_split_file_number;
  //how many splits, this will be modified
  //by check_splitpts_inf_song_length()
  //to really see how many splitpoints we have
  //look at real_splitnumber
  int splitnumber;
  //how many splitpoints we have
  int real_splitnumber;
  //put this function if you want that the library
  //tells you when a file has been split
  //the char* is the filename
  void (*file_split)(const char *,int);
  //for the progress bar
  splt_progress *p_bar;
  //callback for sending the silence level to the client
  void (*get_silence_level)(long time, float level, void *user_data);
  //user data set by the client for the 'get_silence_level' function
  void *silence_level_client_data;
  //sends a message to the main program to tell him what
  //he is doing; the second parameter is the type of split
  void (*put_message)(const char *, splt_message_type );
  //structure in which we have all the splitpoints
  splt_point *points;
  //how many tags we have
  int real_tagsnumber;
  //structure in which we have all the tags
  splt_tags *tags;
  splt_tags tags_like_x;
} splt_struct;

/**********************************/
/* Options structure              */

/**
 * @brief Values for the #SPLT_OPT_SPLIT_MODE option
 *
 * Values for the #SPLT_OPT_SPLIT_MODE option
 */
typedef enum {
  /**
   * Normal split
   */
  SPLT_OPTION_NORMAL_MODE,
  /**
   * Split the file created with mp3wrap or albumwrap
   */
  SPLT_OPTION_WRAP_MODE,
  /**
   * Split with silence detection
   */
  SPLT_OPTION_SILENCE_MODE,
  /**
   * Split with error mode 
   * It is useful to split large file derivated from a concatenation of
   * smaller files
   */
  SPLT_OPTION_ERROR_MODE,
  /**
   * Will create an indefinite number of smaller files with
   * a fixed time length specified by #SPLT_OPT_SPLIT_TIME
   */
  SPLT_OPTION_TIME_MODE,
  /**
   * Split in X pieces of equal time length.
   * X is defined by the #SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER option
   */
  SPLT_OPTION_LENGTH_MODE,
} splt_split_mode_options;

/**
 * @brief Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 *
 * Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 */
typedef enum {
  //output specified by the set_oformat
  SPLT_OUTPUT_FORMAT,
  //the default output
  //it depends of the type of the split
  SPLT_OUTPUT_DEFAULT,
  //we don't change anything, must put the filenames with
  //the functions set_..
  SPLT_OUTPUT_CUSTOM
} splt_output_filenames_options;

/**
 * @brief Default value for the #SPLT_OPT_PARAM_THRESHOLD option
 */
#define SPLT_DEFAULT_PARAM_THRESHOLD -48.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_OFFSET option
 */
#define SPLT_DEFAULT_PARAM_OFFSET 0.8
/**
 * @brief Default value for the #SPLT_OPT_PARAM_MIN_LENGTH option
 */
#define SPLT_DEFAULT_PARAM_MINIMUM_LENGTH 0.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_GAP option
 */
#define SPLT_DEFAULT_PARAM_GAP 30
/**
 * @brief Default value for the #SPLT_OPT_PARAM_NUMBER_TRACKS option
 */
#define SPLT_DEFAULT_PARAM_TRACKS 0

/**
 * @brief Values for the #SPLT_OPT_TAGS option
 *
 * Values for the #SPLT_OPT_TAGS option
 */
typedef enum {
  /**
   * Keep the tags of the original file
   */
  SPLT_TAGS_ORIGINAL_FILE,
  /**
   * Keep the tags issued from cddb, cue or
   * set by the user with #mp3splt_append_tags
   */
  SPLT_CURRENT_TAGS,
  /**
   * Does not put any tags
   */
  SPLT_NO_TAGS
} splt_tags_options;

#define SPLT_ORIGINAL_TAGS_DEFAULT "%[@o,@N=1]"

#define SPLT_DEFAULT_OUTPUT "@f_@mm_@ss_@hh__@Mm_@Ss_@Hh"

/**
 * @brief Default output for the cddb and cue.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_CDDB_CUE_OUTPUT "@A - @n - @t"
/**
 * @brief Default output for the syncerror.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SYNCERROR_OUTPUT "@f_error_@n"
/**
 * @brief Default output for the silence split.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SILENCE_OUTPUT "@f_silence_@n"

//structure with all the options supplied to split the file
typedef struct {
  //this can take the following values:
  //SPLT_OPTION_NORMAL_MODE
  //SPLT_OPTION_WRAP_MODE
  //SPLT_OPTION_SILENCE_MODE
  //SPLT_OPTION_ERROR_MODE
  //SPLT_OPTION_TIME_MODE
  splt_split_mode_options split_mode;

  //might be:
  //SPLT_TAGS_ORIGINAL_FILE - write tags from original file
  //SPLT_NO_TAGS - does not write any tags
  //SPLT_CURRENT_TAGS - tags issued from the cddb or cue for example
  //or that we set manually with the functions
  splt_tags_options tags;

  short xing;

  //defines the output filenames
  splt_output_filenames_options output_filenames;

  //quiet mode: don't perform CRC check or other interaction with the user
  short quiet_mode;

  //Pretend to split the file, without real split: this option works in
  //all modes except error mode and dewrap split.
  short pretend_to_split;

  //frame mode (mp3 only). Process all frames, seeking split positions
  //by counting frames and not with bitrate guessing.
  short option_frame_mode;
  //the time of split when split_mode = OPTION_TIME_SPLIT
  float split_time;
  long overlap_time;
  //this option uses silence detection to auto-adjust splitpoints.
  short option_auto_adjust;
  //input not seekable. enabling this allows you to split mp3 and ogg streams
  //which can be read only one time and canât be seeked.
  //WARNING!
  //if you don't know what this means, set it to FALSE
  short option_input_not_seekable;

  //If this option is SPLT_TRUE, we create directories from the output
  //file names without parsing for illegal characters the output filenames.
  //Otherwise, we parse for illegal characters the filenames and replace
  //them with '_'. The tags are always checked for illegal characters when
  //set into filenames.
  short create_dirs_from_filenames;

  //PARAMETERS---------------------------------------
  //PARAMETERS for option_auto_adjust and option_silence_mode:
  //the sound level to be considered silence
  //(it is a float number between -96 and 0. Default is -48 dB)
  float parameter_threshold;
  //the offset of cutpoint in silence
  //Float number between -2 and 2 and allows you to adjust the offset
  //of cutpoint in silence time.0 is the begin of silence, and 1 the
  //end;default is 0.8. 
  float parameter_offset;

  //PARAMETERS for option_silence_mode:
  //the desired number of tracks
  //(positive integer number of tracks to be split;by default all
  //tracks are split)
  int parameter_number_tracks;
  //the minimum silence length in seconds
  //(positive float of the minimum number of seconds to be considered
  //a valid splitpoint)
  float parameter_minimum_length;
  //allows you to remove the silence between split tracks
  short parameter_remove_silence;

  //PARAMETERS for option_auto_adjust:
  //the gap value around splitpoint to search for silence
  //(positive integer for the time to decode before and after
  //splitpoint;default gap is 30 seconds)
  int parameter_gap;

  //-if we set all the remaining tags (for which we have no tags structure
  //like the tag number 'remaining_tags_like_x'
  //- set to -1 to disable
  int remaining_tags_like_x;

  //if to auto increment the tracknumber of the tags
  int auto_increment_tracknumber_tags;

  /**
   * if we enable the silence points log ('mp3splt.log')
   */
  int enable_silence_log;

  /**
   * If we force the mp3 tags version to 1 or 2 or 1 & 2;
   * Set to 0 if force disable;
   * If disabled, original tags version is set to split files,
   *  or version 1 if compiled without libid3tag support
   */
  int force_tags_version;
  /**
   * the number of files to be created when splitting by equal time length
   */
  int length_split_file_number;
  int replace_tags_in_tags;
} splt_options;

/**********************************/
/* Main structure                 */

//internal structures
typedef struct
{
  //if we have send the message frame mode enabled
  int frame_mode_enabled;
  //if current_refresh_rate = refresh_rate, we call
  //the progress callback
  int current_refresh_rate;
  //if set to SPLT_TRUE,
  //then we don't send messages to clients
  int messages_locked;
  //if we currently use the library, we lock it
  int library_locked;
  //the new filename path (internal)
  char *new_filename_path;
  //used for the normal split
  double split_begin;
  double split_end;
} splt_internal;

/*
 * Structure containing information about one plugin.
 * Must be filled up at plugin initialisation.
 */
typedef struct
{
  float version;
  char *name;
  char *extension;
  char *upper_extension;
} splt_plugin_info;

//contains pointers to the plugin functions
typedef struct {
  int (*check_plugin_is_for_file)(void *state, int *error);
  void (*set_plugin_info)(splt_plugin_info *info, int *error);
  void (*search_syncerrors)(void *state, int *error);
  void (*dewrap)(void *state, int listonly, const char *dir, int *error);
  void (*set_total_time)(void *state, int *error);
  int (*simple_split)(void *state, const char *output_fname, off_t begin, off_t end);
  double (*split)(void *state, const char *final_fname, double begin_point,
      double end_point, int *error, int save_end_point);
  int (*scan_silence)(void *state, int *error);
  void (*set_original_tags)(void *state, int *error);
  void (*init)(void *state, int *error);
  void (*end)(void *state, int *error);
} splt_plugin_func;

//structure containing all the data about one plugin
typedef struct
{
  splt_plugin_info info;
  //complete filename of the plugin shared object
  char *plugin_filename;
  //plugin handle get with lt_dlopen
  //-would be closed with lt_dlclose
  void *plugin_handle;
  //plugin functions
  splt_plugin_func *func;
} splt_plugin_data;

//internal plugins structure
typedef struct
{
  //directories where we scan for plugins
  char **plugins_scan_dirs;
  int number_of_dirs_to_scan;
  //the number of plugins found
  int number_of_plugins_found;
  //data structure about all the plugins
  splt_plugin_data *data;
} splt_plugins;

//structure containing error strings for error messages
typedef struct
{
  char *error_data;
  char *strerror_msg;
} splt_error;

//structure for the splt state
typedef struct {

  //if we cancel split or not
  //set to SPLT_TRUE cancels the split
  short cancel_split;
  //filename to split
  char *fname_to_split;
  //where the split file will be split
  char *path_of_split;

  //if this is non null, we write a m3u from the split files
  char *m3u_filename;

  //tags of the original file to split
  splt_tags original_tags;

  //options for the split
  splt_options options;
  //split related
  splt_struct split;
  //output format  
  splt_oformat oformat;
  //wrap related
  splt_wrap *wrap;
  //syncerror related
  splt_syncerrors *serrors;
  //counter for the number of sync errors found
  //-the state->serros->serrors_points_num must be used when processing the
  //syncerrors from 'serrors' 
  //-this is just a standalone counter
  unsigned long syncerrors;
  //freedb related
  splt_freedb fdb;

  //internal options
  splt_internal iopts;

  //see the ssplit structure
  struct splt_ssplit *silence_list;

  //proxy infos
  //splt_proxy proxy;

  //file format states, mp3,ogg..
  void *codec;

  //error strings for error code messages
  splt_error err;

  //plugins structure
  splt_plugins *plug;
  int current_plugin;

  //filename of the silence log: 'mp3splt.log' in the original mp3splt
  char *silence_log_fname;
} splt_state;

/*****************************************/
/* Confirmations, errors and messages    */

typedef enum {
  SPLT_OK = 0,

  SPLT_OK_SPLIT = 1,
  SPLT_SPLITPOINT_BIGGER_THAN_LENGTH = 4,
  SPLT_SILENCE_OK = 5,
  SPLT_TIME_SPLIT_OK = 6,
  SPLT_NO_SILENCE_SPLITPOINTS_FOUND = 7,
  SPLT_OK_SPLIT_EOF = 8,
  SPLT_LENGTH_SPLIT_OK = 9,

  SPLT_FREEDB_OK = 100,
  SPLT_FREEDB_FILE_OK = 101,
  SPLT_CDDB_OK = 102,
  SPLT_CUE_OK = 103,
  SPLT_FREEDB_MAX_CD_REACHED = 104,
  SPLT_AUDACITY_OK = 105,

  SPLT_DEWRAP_OK = 200,

  SPLT_SYNC_OK = 300,
  SPLT_MIGHT_BE_VBR = 301,

  SPLT_ERR_SYNC = -300,
  SPLT_ERR_NO_SYNC_FOUND = -301,
  SPLT_ERR_TOO_MANY_SYNC_ERR = -302,

  SPLT_OUTPUT_FORMAT_OK = 400,
  SPLT_OUTPUT_FORMAT_AMBIGUOUS = 401,

  SPLT_ERROR_SPLITPOINTS = -1,
  SPLT_ERROR_CANNOT_OPEN_FILE = -2,
  SPLT_ERROR_INVALID = -3,
  SPLT_ERROR_EQUAL_SPLITPOINTS = -5,
  SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER = -6,
  SPLT_ERROR_NEGATIVE_SPLITPOINT = -7,
  SPLT_ERROR_INCORRECT_PATH = -8,
  SPLT_ERROR_INCOMPATIBLE_OPTIONS = -10,
  SPLT_ERROR_INPUT_OUTPUT_SAME_FILE = -12,
  SPLT_ERROR_CANNOT_ALLOCATE_MEMORY = -15,
  SPLT_ERROR_CANNOT_OPEN_DEST_FILE = -16,
  SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE = -17,
  SPLT_ERROR_WHILE_READING_FILE = -18,
  SPLT_ERROR_SEEKING_FILE = -19,
  SPLT_ERROR_BEGIN_OUT_OF_FILE = -20,
  SPLT_ERROR_INEXISTENT_FILE = -21,
  SPLT_SPLIT_CANCELLED = -22,
  SPLT_ERROR_LIBRARY_LOCKED = -24,
  SPLT_ERROR_STATE_NULL = -25,
  SPLT_ERROR_NEGATIVE_TIME_SPLIT = -26,
  SPLT_ERROR_CANNOT_CREATE_DIRECTORY = -27,
  SPLT_ERROR_CANNOT_CLOSE_FILE = -28,
  SPLT_ERROR_NO_PLUGIN_FOUND = -29,
  SPLT_ERROR_CANNOT_INIT_LIBLTDL = -30,
  SPLT_ERROR_CRC_FAILED = -31,
  SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE = -32,
  SPLT_ERROR_PLUGIN_ERROR = -33,
  SPLT_ERROR_TIME_SPLIT_VALUE_INVALID = -34,
  SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID = -35,
  SPLT_ERROR_CANNOT_GET_TOTAL_TIME = -36,

  SPLT_FREEDB_ERROR_INITIALISE_SOCKET = -101,
  SPLT_FREEDB_ERROR_CANNOT_GET_HOST = -102,
  SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET = -103,
  SPLT_FREEDB_ERROR_CANNOT_CONNECT = -104,
  SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE = -105,
  SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER = -106,
  SPLT_FREEDB_ERROR_SITE_201 = -107,
  SPLT_FREEDB_ERROR_SITE_200 = -108,
  SPLT_FREEDB_ERROR_BAD_COMMUNICATION = -109,
  SPLT_FREEDB_ERROR_GETTING_INFOS = -110,
  SPLT_FREEDB_NO_CD_FOUND = -111,
  SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE = -112,
  SPLT_INVALID_CUE_FILE = -115,
  SPLT_INVALID_CDDB_FILE = -116,
  SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE = -118,
  SPLT_FREEDB_ERROR_SITE = -119,

  SPLT_DEWRAP_ERR_FILE_LENGTH = -200,
  SPLT_DEWRAP_ERR_VERSION_OLD = -201,
  SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX = -202,
  SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE = -203,
  SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED = -204,

  SPLT_OUTPUT_FORMAT_ERROR = -400,

  SPLT_ERROR_INEXISTENT_SPLITPOINT = -500,

  SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE = -600,

  SPLT_INVALID_AUDACITY_FILE = -700,
} splt_code;

//internal
#define SPLT_INTERNAL_PROGRESS_RATE 1
#define SPLT_INTERNAL_FRAME_MODE_ENABLED 2

//progress messages
/**
 * @brief Progress messages sent from the library to the client
 *
 * Progress messages sent from the library to the client
 */
typedef enum {
  /**
   * Preparing to split a song
   */
  SPLT_PROGRESS_PREPARE,
  /**
   * Creating the split file
   */
  SPLT_PROGRESS_CREATE,
  /**
   * Searching for syncerrors
   */
  SPLT_PROGRESS_SEARCH_SYNC,
  /**
   * Scanning for silence
   */
  SPLT_PROGRESS_SCAN_SILENCE
} splt_progress_messages;

//options types: integer
/**
 * @brief Integer options
 *
 * Integer options
 *
 * Use #mp3splt_set_int_option to set those options\n
 * Use #mp3splt_get_int_option to get those options
 */
typedef enum {
  /**
   * Pretend to split the file, without real split: this option works in
   * all modes except error mode and dewrap split.
   */
  SPLT_OPT_PRETEND_TO_SPLIT,
  /*
   * If quiet; we don't do CRC check or human interaction
   */
  SPLT_OPT_QUIET_MODE,
  /**
   * If we print out debug messages
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_DEBUG_MODE,
  /**
   * The type of the split
   *
   * The option can take the values from #splt_split_mode_options
   *
   * Default is #SPLT_OPTION_NORMAL_MODE
   */
  SPLT_OPT_SPLIT_MODE,
  /**
   * The type of tags to put in the new split files
   *
   * The option can take the values from #splt_tags_options
   *
   * Default is #SPLT_CURRENT_TAGS
   */
  SPLT_OPT_TAGS,
  /**
   * #SPLT_TRUE if we write Xing header (mp3 only)
   */
  SPLT_OPT_XING,
  /**
   * If this option is SPLT_TRUE, we create directories from the output
   * file names without parsing for illegal characters the output filenames.
   *
   * Otherwise, we parse for illegal characters the filenames and replace
   * them with '_'. The tags are always checked for illegal characters when
   * set into filenames.
   *
   * Default is #SPLT_FALSE because it's safer to check for illegal
   * characters.
   */
  SPLT_OPT_CREATE_DIRS_FROM_FILENAMES,
  /**
   * The option can take the values from #SPLT_OUTPUT_FILENAMES_OPTIONS
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_OUTPUT_FILENAMES,
  /**
   * If we enable the frame mode or not \n
   * The frame mode processes the file frame by frame and
   * it is useful when splitting a VBR (Variable Bit Rate) file
   * 
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_TRUE
   */
  SPLT_OPT_FRAME_MODE,
  /**
   * If we use silence detection to auto-adjust splitpoints\n
   * The following options may change the behaviour of the
   * auto-adjust: #SPLT_OPT_PARAM_THRESHOLD, #SPLT_OPT_PARAM_OFFSET,
   * #SPLT_OPT_PARAM_GAP, #
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_AUTO_ADJUST,
  /**
   * If the input is not seekable\n
   * This allows you to split mp3 and ogg streams which can be read
   * only one time and can't be seeked
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_INPUT_NOT_SEEKABLE,
  /**
   * The desired number of tracks when having a
   * #SPLT_OPTION_SILENCE_MODE split
   *
   * The option can take positive integer values. 0 means that we
   * split as many files we found
   *
   * Default is #SPLT_DEFAULT_PARAM_TRACKS
   */
  SPLT_OPT_PARAM_NUMBER_TRACKS,
  /**
   * Allows you to remove the silence between the split tracks when
   * having a #SPLT_OPTION_SILENCE_MODE split
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_PARAM_REMOVE_SILENCE,
  /**
   * The time to auto-adjust before and after splitpoint
   * when having the #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take positive integer values
   *
   * Default is #SPLT_DEFAULT_PARAM_GAP
   */
  SPLT_OPT_PARAM_GAP,
  /**
   * if to set all tags like X one
   */
  SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X,
  /**
   * -if we auto increment the tracknumber in the tags
   */
  SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS,
  /**
   * if we enable the silence points log ('mp3splt.log')
   */
  SPLT_OPT_ENABLE_SILENCE_LOG,
  /**
   * if we force a tags version or not
   */
  SPLT_OPT_FORCE_TAGS_VERSION,
  /**
   * number of files to split by equal time length
   */
  SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER,
  /**
   *
   */
  SPLT_OPT_REPLACE_TAGS_IN_TAGS
} splt_int_options;

//options types: long
/**
 * @brief Integer options
 *
 * Integer options
 *
 * Use #mp3splt_set_long_option to set those options\n
 * Use #mp3splt_get_long_option to get those options
 */
typedef enum {
  /** 
   * Time to overlap between the split files
   */
  SPLT_OPT_OVERLAP_TIME
} splt_long_options;

//option types: float
/**
 * @brief Float options
 *
 * Float options
 *
 * Use #mp3splt_set_float_option to set those options\n
 * Use #mp3splt_get_float_option to get those options
 */
typedef enum {
  /**
   * The interval for the #SPLT_OPTION_TIME_MODE split (in
   * hundreths of seconds)
   *
   * The option can take positive float values
   *
   * Default is 6000 hundreths of seconds (one minute)
   */
  SPLT_OPT_SPLIT_TIME,
  /**
   * The threshold  level (dB) to be considered silence\n
   * It is a float number between -96 and 0. It is used when
   * having a #SPLT_OPTION_SILENCE_MODE split or when having the
   * #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take float values between -96 and 0
   *
   * Default is #SPLT_DEFAULT_PARAM_THRESHOLD
   */
  SPLT_OPT_PARAM_THRESHOLD,
  /**
   * Allows you to adjust the offset of cutpoint in silence time when
   * having a #SPLT_OPTION_SILENCE_MODE split or when having the
   * #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take float values between -2 and 2\n
   * 0  is  the begin of silence, and 1 the end
   *
   * Default is #SPLT_DEFAULT_PARAM_OFFSET
   */
  SPLT_OPT_PARAM_OFFSET,
  /**
   * Minimum number of seconds to be considered a valid splitpoint\n
   * All silences shorter than this value are discarded.
   *
   * The option can take positive float values
   *
   * Default is #SPLT_DEFAULT_PARAM_MINIMUM_LENGTH
   */
  SPLT_OPT_PARAM_MIN_LENGTH
} splt_float_options;


/**
 * we define a 'skippoint' as a splitpoint that is not taken into
 * consideration
 */
typedef enum {
  /* a regular splitpoint */
  SPLT_SPLITPOINT,
  /* a skippoint */
  SPLT_SKIPPOINT,
} splt_type_of_splitpoint;

/**
 * Freedb constants
 */

/*
 * freedb2 search type
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI 1
/*
 * freedb search type
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB 2
/*
 * freedb get file type
 * we retrieve the file by using the cddb.cgi script
 * (usually on port 80)
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI 3
/*
 * we retrieve the file by using the freedb cddb protocol 
 * (usually on port 8880)
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB 4
/**
 * default port
 */
#define SPLT_FREEDB_CDDB_CGI_PORT 80
/**
 * default port
 */
#define SPLT_FREEDB_CDDB_PORT 8880
/**
 * urls of freedb2.org and freedb.org
 */
#define SPLT_FREEDB_CGI_SITE "freedb.org/~cddb/cddb.cgi"
#define SPLT_FREEDB2_CGI_SITE "tracktype.org/~cddb/cddb.cgi"

//package information constants
#ifndef SPLT_PACKAGE_NAME
/**
 * @brief Package name
 */
#define SPLT_PACKAGE_NAME "libmp3splt"
#endif
#ifndef SPLT_PACKAGE_VERSION
/**
 * @brief Package version
 */
#define SPLT_PACKAGE_VERSION "0.5.9"
#endif
/**
 * @brief Package authors
 */
#define SPLT_AUTHOR "Matteo Trotta | Munteanu Alexandru"
#define SPLT_EMAIL "<mtrotta@users.sourceforge.net> | <io_fx@yahoo.fr>"
/**
 * @brief Package website
 */
#define SPLT_WEBSITE "http://mp3splt.sourceforge.net"

/* other useful variables */

#define MP3SPLT_LIB_GETTEXT_DOMAIN "libmp3splt"

//backslash character
#ifndef SPLT_DIRCHAR
#ifdef __WIN32__
#define SPLT_DIRCHAR '\\'
#define SPLT_NDIRCHAR '/'
#else
#define SPLT_DIRCHAR '/'
#define SPLT_NDIRCHAR '\\'
#endif
#endif

/**
 * @file mp3splt.h
 * 
 * @brief main functions
 * 
 * Contains the library API functions.
 */

/**
 * Initialisation and free
 */

/**
 * @brief Creates a new state structure
 *
 * Creates a new state structure, needed by libmp3splt
 */
splt_state *mp3splt_new_state(int *error);

//find plugins
int mp3splt_find_plugins(splt_state *state);

//this function frees the left variables in the library
//don't forget to call this function ONLY at the end of the program
//returns possible error
void mp3splt_free_state(splt_state *state, int *error);

/************************************/
/* Set path                         */

//puts the path for the new split files
//returns possible error
int mp3splt_set_path_of_split(splt_state *state, const char *path);

/************************************/
/* Set filename                     */

//put the filename to split
//returns possible error
int mp3splt_set_filename_to_split(splt_state *state, const char *filename);
int mp3splt_set_m3u_filename(splt_state *state, const char *filename);
int mp3splt_set_silence_log_filename(splt_state *state, const char *filename);

/************************************/
/* Set callback functions           */

int mp3splt_set_message_function(splt_state *state, 
    void (*message_cb)(const char *, splt_message_type));
int mp3splt_set_split_filename_function(splt_state *state,
    void (*file_cb)(const char *,int));
int mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar));
int mp3splt_set_silence_level_function(splt_state *state,
  void (*get_silence_cb)(long time, float level, void *user_data),
  void *user_data);

/************************************/
/* Splitpoints                      */

//puts a splitpoint
//returns possible error
int mp3splt_append_splitpoint(splt_state *state,
    long split_value, const char *name, int type);

//returns a pointer to all the current splitpoints
const splt_point *mp3splt_get_splitpoints(splt_state *state,
    int *splitpoints_number, int *error);

//erase all the splitpoints
void mp3splt_erase_all_splitpoints(splt_state *state,
    int *error);

/************************************/
/* Tags                             */

//puts a tag
int mp3splt_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre);

//returns a pointer to all the current tags
const splt_tags *mp3splt_get_tags(splt_state *state,
    int *tags_number, int *error);

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, const char *tags,
    int *error);

void mp3splt_erase_all_tags(splt_state *state,
    int *error);

/************************************/
/* Options                          */

int mp3splt_set_int_option(splt_state *state, int option_name,
    int value);

int mp3splt_set_long_option(splt_state *state, int option_name,
    long value);

int mp3splt_set_float_option(splt_state *state, int option_name,
    float value);

int mp3splt_get_int_option(splt_state *state, int option_name,
    int *error);

long mp3splt_get_long_option(splt_state *state, int option_name,
    int *error);

float mp3splt_get_float_option(splt_state *state, int option_name,
    int *error);

/************************************/
/* Split functions                  */

//split a ogg or mp3 file
//returns possible error
int mp3splt_split(splt_state *state);

//cancel split function
//returns possible error
void mp3splt_stop_split(splt_state *state,
    int *error);

/************************************/
/*    Cddb and Cue functions        */

//get the cue splitpoints from a file and puts them in the state
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
    const char *cue_file, int *error);

//read cddb splitpoints from file and puts them in the state
void mp3splt_put_cddb_splitpoints_from_file(splt_state *state,
    const char *cddb_file, int *error);

void mp3splt_put_audacity_labels_splitpoints_from_file(splt_state *state,
    const char *file, int *error);

/************************************/
/*    Freedb functions              */

//returns the freedb results and possible eerror
/**
 * @brief test
 */
const splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    const char *searched_string,
    int *error,
    int search_type,
    const char *search_server,
    int port);

void mp3splt_write_freedb_file_result(splt_state *state,
    int disc_id,
    const char *cddb_file,
    int *error,
    int cddb_get_type,
    const char *cddb_get_server,
    int port);

void mp3splt_export_to_cue(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error);

void mp3splt_set_oformat(splt_state *state,
    const char *format_string, int *error);

/************************************/
/* Other utilities                  */

//counts the number of tracks found with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error);

int mp3splt_set_silence_points(splt_state *state, int *error);

//returns the version of libmp3splt
void mp3splt_get_version(char *version);

//result must be freed
char *mp3splt_get_strerror(splt_state *state, int error_code);

//returns the number of syncerrors
//puts possible error in error variable
const splt_syncerrors *mp3splt_get_syncerrors(splt_state *state,
    int *error);

//returns the wrapped files found
const splt_wrap *mp3splt_get_wrap_files(splt_state *state, int *error);

int mp3splt_append_plugins_scan_dir(splt_state *state, char *dir);

#ifdef __WIN32__
char *mp3splt_win32_utf16_to_utf8(const wchar_t *source);
#endif

char **mp3splt_find_filenames(splt_state *state, const char *filename,
    int *num_of_files_found, int *error);

int mp3splt_u_check_if_directory(const char *fname);

#define MP3SPLT_MP3SPLT_H

#endif

