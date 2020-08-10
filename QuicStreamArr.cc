//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "QuicStreamArr.h"
#include "IndexQueueNodes_m.h"
#include <iostream>


namespace inet {

/*
 * we create a new empty stream array and insert all the cells to the free index queue (because we didn't open any stream yet.
 */
QuicStreamArr::QuicStreamArr(int streams_num) {
    stream_arr_= new stream[streams_num];
    max_streams_num_=streams_num;
    valid_streams_num_=0;
    curr_max_stream_=0;
    for (int i=0; i<streams_num; i++){
        IndexQueueNodes *queue_node;
        queue_node=new IndexQueueNodes;
        queue_node->setIndex(i);
        this->free_index_queue_.insert(queue_node);
    }
}


int QuicStreamArr::AddNewStream(int max_bytes,int quanta, int total_bytes_to_send){

    IndexQueueNodes *index_queue_node= (IndexQueueNodes*)free_index_queue_.pop();//  get an index for a cell in the array where i can create a new stream in.
    int free_index=index_queue_node->getIndex();
    this->stream_arr_[free_index].sent_bytes_num=0;
    this->stream_arr_[free_index].max_bytes=max_bytes;
    this->stream_arr_[free_index].stream_id=this->curr_max_stream_;
    this->stream_arr_[free_index].valid=true;
    this->stream_arr_[free_index].curr_quanta=quanta;
    this->stream_arr_[free_index].total_bytes_to_send = total_bytes_to_send;
    this->stream_arr_[free_index].bytes_left_to_send = total_bytes_to_send;
    this->curr_max_stream_++;
    this->valid_streams_num_++;
    this->avilable_streams_queue_.insert(index_queue_node);//add the index of the new stream created to the avilable streams queue.
    this->total_free_bytes_+=max_bytes;
    return free_index;
}

bool QuicStreamArr::CloseStream(int stream_id){
    for (int i=0; i<this->max_streams_num_;i++){
        if (this->stream_arr_[i].stream_id==stream_id){
            IndexQueueNodes *queue_node;
            queue_node=new IndexQueueNodes;
            queue_node->setIndex(i);
            this->free_index_queue_.insert(queue_node);//we add the index of the cell to the free index queue so we can later override it with a new stream.
            this->valid_streams_num_--;
            this->stream_arr_[i].valid=false;
            return true;
        }
    }
    return false;
}

bool QuicStreamArr::IsAvilableStreamExist(){
    if(this->avilable_streams_queue_.isEmpty()){
        return false;
    }
    return true;
}

StreamsData* QuicStreamArr::DataToSend(int frames_number ,int bytes_in_packet){
    bool packet_full = false;
    int current_free_bytes = bytes_in_packet;
    StreamsData* new_data=new StreamsData(frames_number);
    for (int i=0; i<frames_number; i++){
        //int bytes_to_send;
        IndexQueueNodes *index_queue_node= (IndexQueueNodes*)avilable_streams_queue_.pop();//find new available stream.
        int stream_free_index=index_queue_node->getIndex();
      //  int free_bytes_in_stream=this->stream_arr_[stream_free_index].max_bytes-this->stream_arr_[stream_free_index].bytes_num;
        int frame_bytes;
        if (stream_arr_[stream_free_index].bytes_left_to_send <= stream_arr_[stream_free_index].max_bytes)
            frame_bytes = stream_arr_[stream_free_index].bytes_left_to_send;
        else
            frame_bytes = stream_arr_[stream_free_index].max_bytes;

        if (frame_bytes >= current_free_bytes) {
            frame_bytes = current_free_bytes;
            packet_full = true;
        }


     //   if (free_bytes_in_stream> this->stream_arr_[stream_free_index].curr_quanta){
          //  bytes_to_send=this->stream_arr_[stream_free_index].curr_quanta;
           // this->avilable_streams_queue_.insert(index_queue_node);

      //  }

        stream_arr_[stream_free_index].bytes_left_to_send -= frame_bytes;
        int stream_id=this->stream_arr_[stream_free_index].stream_id;
        int offset=this->stream_arr_[stream_free_index].sent_bytes_num;
        if (stream_arr_[stream_free_index].total_bytes_to_send == offset + frame_bytes)
            new_data->AddNewFrame(i, stream_id, offset, frame_bytes, true);
        else
            new_data->AddNewFrame(i, stream_id, offset, frame_bytes, false);
    //    total_bytes_to_send-=bytes_to_send;
        this->stream_arr_[stream_free_index].sent_bytes_num+=frame_bytes;
      //  if (total_bytes_to_send==0){
       //  return new_data;
        if (packet_full)
            return new_data;
        else
            current_free_bytes -= frame_bytes;
    }
    return new_data;
}

QuicStreamArr::~QuicStreamArr() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */
